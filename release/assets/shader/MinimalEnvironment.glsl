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
layout (location = 0) out vec4 outAlbedoDepth;


// uniforms
layout (binding = 0) uniform sampler2D textureGBufferPosition;
layout (binding = 1) uniform sampler2D textureGBufferNormal;
layout (binding = 2) uniform sampler2D textureGBufferDiffuse;
layout (binding = 3) uniform sampler2D textureGBufferEmission;
layout (binding = 4) uniform sampler2D textureGBufferSpecularShininess;
layout (binding = 5) uniform sampler2D textureGBufferFaceNormal;
layout (binding = 6) uniform sampler2D textureSSAO;
layout (binding = 7) uniform sampler2DArray textureShadowMap;
uniform float ambientOcclusionEnhance;


// fragment shader main
void main(void){
    GBuffer gb = GetGBuffer(textureGBufferPosition, textureGBufferNormal, textureGBufferDiffuse, textureGBufferEmission, textureGBufferSpecularShininess, textureGBufferFaceNormal, texCoord);
    AtmosphereParameter atmosphere = GetAtmosphereParameter();
    vec3 viewDirection = normalize(cameraToFragment);
    vec3 viewDirectionWorldSpace = normalize(cameraToFragmentWorldSpace);

    // ambient occlusion
    #ifdef PRISMA_CONFIGURATION_ENABLE_AMBIENT_OCCLUSION
    float ambientOcclusion = SSAOBlur(textureSSAO, texCoord);
    ambientOcclusion = clamp(ambientOcclusion - ambientOcclusionEnhance*(1.0f - ambientOcclusion), 0.0f, 1.0f);
    gb.color *= ambientOcclusion;
    #endif

    // shadow mapping
    #ifdef PRISMA_CONFIGURATION_ENABLE_SHADOW_MAPPING
    float shadowFactor = 1.0f - ShadowCalculation(textureShadowMap, gb.position, gb.faceNormal, atmosphere.sunLightDirection);
    #else
    float shadowFactor = 1.0;
    #endif

    // lighting
    vec3 lightenObjectColor = vec3(0.0f);
    lightenObjectColor += AllDirectionalLights(gb, viewDirection);
    lightenObjectColor += AllPointLights(gb, viewDirection);
    lightenObjectColor += AllSpotLights(gb, viewDirection);
    lightenObjectColor += shadowFactor * DirectionalLight(gb, viewDirection, atmosphere.sunColor, atmosphere.sunLightDirection);
    lightenObjectColor += ambientLightColor * gb.color;
    lightenObjectColor += gb.emission;

    // atmosphere, sky color
    atmosphere.sunDiscIntensity = 0.0f; // remove sun disc for reflection/refraction rendering
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

    // final albedo and depth
    outAlbedoDepth = vec4(fragColor, depth);
}


#endif /* FRAGMENT_SHADER */

