$PRISMA_CONFIGURATION_MACROS$
#include "CameraUniform.glsl"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERTEX SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef VERTEX_SHADER


// vertex shader input
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 textureCoord;


// vertex shader output
out vec2 texCoord;
out vec3 cameraToFragment;
out vec3 cameraToFragmentWorldSpace;


// vertex shader main
void main(void){
    texCoord = textureCoord;
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    vec4 cameraToFragmentTimesW = cameraInvProjectionMatrix * gl_Position;
    mat3 RotateViewToWorld = transpose(mat3(cameraViewMatrix));
    cameraToFragment = cameraToFragmentTimesW.xyz / cameraToFragmentTimesW.w;
    cameraToFragmentWorldSpace = RotateViewToWorld * cameraToFragment;
}


#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FRAGMENT SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef FRAGMENT_SHADER
#include "GBufferStruct.glsl"
#include "AtmosphereUniform.glsl"
#include "LightMatricesUniform.glsl"
#include "Lighting.glsl"
#include "SkyModel.glsl"
#include "SSAODetails.glsl"
#include "CascadedShadowMapping.glsl"


// fragment shader input
in vec2 texCoord;
in vec3 cameraToFragment;
in vec3 cameraToFragmentWorldSpace;


// fragment shader output
layout (location = 0) out vec3 outAlbedo;
layout (location = 1) out vec3 outBrightness;


// uniforms
layout (binding = 0) uniform sampler2D textureGBufferPosition;
layout (binding = 1) uniform sampler2D textureGBufferNormal;
layout (binding = 2) uniform sampler2D textureGBufferDiffuse;
layout (binding = 3) uniform sampler2D textureGBufferEmission;
layout (binding = 4) uniform sampler2D textureGBufferSpecularShininess;
layout (binding = 5) uniform sampler2D textureGBufferFaceNormal;
layout (binding = 6) uniform sampler2D textureSSAO;
layout (binding = 7) uniform sampler2DArray textureShadowMap;
uniform float brightnessThreshold;
uniform float ambientOcclusionEnhance;
uniform float removeShadowOnWater;


// constants
const vec3 luma = vec3(0.2126f, 0.7152f, 0.0722f);


// fragment shader main
void main(void){
    GBuffer gb = GetGBuffer(textureGBufferPosition, textureGBufferNormal, textureGBufferDiffuse, textureGBufferEmission, textureGBufferSpecularShininess, textureGBufferFaceNormal, texCoord);
    AtmosphereParameter atmosphere = GetAtmosphereParameter();
    vec3 viewDirection = normalize(cameraToFragment);
    vec3 viewDirectionWorldSpace = normalize(cameraToFragmentWorldSpace);

    // ambient occlusion (only if gb.color is non-reflective)
    #ifdef PRISMA_CONFIGURATION_ENABLE_AMBIENT_OCCLUSION
    float ambientOcclusion = SSAOBlur(textureSSAO, texCoord);
    ambientOcclusion = clamp(ambientOcclusion - ambientOcclusionEnhance*(1.0f - ambientOcclusion), 0.0f, 1.0f);
    gb.color = mix(gb.color * ambientOcclusion, gb.color, gb.reflectionIndicator);
    #endif

    // shadow mapping
    #ifdef PRISMA_CONFIGURATION_ENABLE_SHADOW_MAPPING
    float shadowFactor = 1.0f - ShadowCalculation(textureShadowMap, gb.position, gb.faceNormal, atmosphere.sunLightDirection);
    #else
    float shadowFactor = 1.0f;
    #endif

    // lighting (for reflecting surfaces set diffuse color to zero)
    vec3 waterColor = gb.color;
    gb.color = mix(gb.color, vec3(0.0f), gb.reflectionIndicator);
    vec3 lightenObjectColor = vec3(0.0f);
    lightenObjectColor += AllDirectionalLights(gb, viewDirection);
    lightenObjectColor += AllPointLights(gb, viewDirection);
    lightenObjectColor += AllSpotLights(gb, viewDirection);
    lightenObjectColor += shadowFactor * DirectionalLight(gb, viewDirection, atmosphere.sunColor, atmosphere.sunLightDirection);
    lightenObjectColor += ambientLightColor * gb.color;
    lightenObjectColor += gb.emission;
    lightenObjectColor = mix(lightenObjectColor, (waterColor + lightenObjectColor) * max(shadowFactor, removeShadowOnWater), gb.reflectionIndicator);

    // atmosphere, sky color
    vec3 skyColor = SkyModel(viewDirectionWorldSpace, atmosphere);

    // final fragment color shading
    vec3 fragColor = lightenObjectColor;
    float clipFar = cameraClippingInfo.y;
    float depth = sqrt(dot(gb.position, gb.position));
    float blendOut = smoothstep(0.0f, 1.0f, clamp((depth - atmosphere.depthBlendOutDistanceMin) / atmosphere.depthBlendOutDistanceRange, 0.0f, 1.0f));
    fragColor = mix(fragColor, skyColor, blendOut);
    if(-gb.position.z > clipFar){
        fragColor = skyColor;
    }

    // fog (simple linear depth fog)
    float fogFactor = atmosphere.fogDensityBegin + (atmosphere.fogDensityEnd - atmosphere.fogDensityBegin) * smoothstep(atmosphere.fogDepthBegin, atmosphere.fogDepthEnd, depth);
    fragColor = mix(fragColor, atmosphere.depthFogColor, fogFactor);

    // final albedo and brightness color
    outAlbedo = fragColor;
    outBrightness = fragColor * max(sign(dot(fragColor, luma) - brightnessThreshold), 0.0f);

    // for debugging
    // outAlbedo = outBrightness;
    // outAlbedo = vec3(ambientOcclusion);
    // outAlbedo = vec3(0.0, 1.0, 0.0);
    // outAlbedo = texture(textureGBufferPosition, texCoord).rgb;
    // outAlbedo = texture(textureGBufferNormal, texCoord).rgb;
    // outAlbedo = texture(textureGBufferFaceNormal, texCoord).rgb;
    // outAlbedo = texture(textureGBufferDiffuse, texCoord).rgb;
    // outAlbedo = texture(textureGBufferEmission, texCoord).rgb;
    // outAlbedo = texture(textureGBufferSpecularShininess, texCoord).rgb;
    // outAlbedo = vec3(texture(textureShadowMap, vec3(texCoord, 0)).r);
}


#endif /* FRAGMENT_SHADER */

