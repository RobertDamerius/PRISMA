#include "CameraUniform.glsl"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERTEX SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef VERTEX_SHADER


// vertex shader input
layout (location = 0) in vec2 position;


// vertex shader output
out VS_OUT {
    vec3 position;
    vec2 texCoord1;
    vec2 texCoord2;
    vec3 tangent;
} vsOut;


// uniforms
uniform mat4 modelMatrix;
uniform float textureCoordinateScaling1;
uniform float textureCoordinateScaling2;
uniform vec2 textureCoordinateAnimation1;
uniform vec2 textureCoordinateAnimation2;


// vertex shader main
void main(void){
    // transformation matrices
    mat4 viewModelMatrix = cameraViewMatrix * modelMatrix;

    // vertex shader output to fragment shader
    vec4 viewSpacePosition = viewModelMatrix * vec4(position, 0.0f, 1.0f);
    vsOut.position = viewSpacePosition.xyz;
    vsOut.texCoord1 = position * textureCoordinateScaling1 + textureCoordinateAnimation1;
    vsOut.texCoord2 = position * textureCoordinateScaling2 + textureCoordinateAnimation2;
    vsOut.tangent = mat3(viewModelMatrix) * vec3(1.0f, 0.0f, 0.0f);

    // fragment position on screen
    gl_Position = cameraProjectionMatrix * viewSpacePosition;
}


#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// GEOMETRY SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef GEOMETRY_SHADER


// layout specification
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


// geometry shader input
in VS_OUT {
    vec3 position;
    vec2 texCoord1;
    vec2 texCoord2;
    vec3 tangent;
} gsIn[];


// geometry shader output
out GS_OUT {
    vec3 position;
    vec2 texCoord1;
    vec2 texCoord2;
    vec3 normal;
    vec3 tangent;
    vec4 clipSpacePosition;
} gsOut;


// geometry shader main
void main(void){
    // calculate face normal
    vec3 edgeA = gsIn[1].position - gsIn[0].position;
    vec3 edgeB = gsIn[2].position - gsIn[1].position;
    vec3 faceNormal = normalize(cross(edgeA, edgeB));

    // forward triangle including face normal
    for(int i = 0; i < 3; ++i){
        gl_Position = gl_in[i].gl_Position;
        gsOut.position = gsIn[i].position;
        gsOut.texCoord1 = gsIn[i].texCoord1;
        gsOut.texCoord2 = gsIn[i].texCoord2;
        gsOut.normal = faceNormal;
        gsOut.tangent = gsIn[i].tangent;
        gsOut.clipSpacePosition = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}


#endif /* GEOMETRY_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FRAGMENT SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef FRAGMENT_SHADER


// fragment shader input
in GS_OUT {
    vec3 position;
    vec2 texCoord1;
    vec2 texCoord2;
    vec3 normal;
    vec3 tangent;
    vec4 clipSpacePosition;
} fsIn;


// fragment shader output
layout (location = 0) out vec3 gBufferPosition;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferDiffuse;
layout (location = 3) out vec4 gBufferEmissionReflection;
layout (location = 4) out vec4 gBufferSpecularShininess;
layout (location = 5) out vec3 gBufferFaceNormal;


// uniforms
layout (binding = 0) uniform sampler2D textureWaterReflection;
layout (binding = 1) uniform sampler2D textureWaterRefraction;
layout (binding = 2) uniform sampler2D textureDuDvMap;
layout (binding = 3) uniform sampler2D textureNormalMap;
uniform vec3 specularColor;
uniform float shininess;
uniform vec2 strengthDuDvNormal1;
uniform vec2 strengthDuDvNormal2;
uniform vec3 reflectionColorMultiplier;
uniform vec3 refractionColorMultiplier;
uniform float distortionWaterDepthScale;


// constants
const float normalMapCenterRG = 128.0f / 255.0f;


// fragment shader main
void main(void){
    // Input texture maps
    vec2 dudvMap1 = (2.0f * texture(textureDuDvMap, fsIn.texCoord1).rg - 1.0f);
    vec2 dudvMap2 = (2.0f * texture(textureDuDvMap, fsIn.texCoord2).rg - 1.0f);
    vec3 normalMap1 = texture(textureNormalMap, fsIn.texCoord1).rgb;
    vec3 normalMap2 = texture(textureNormalMap, fsIn.texCoord2).rgb;
    vec3 normalTextureSpace1 = normalize(vec3((normalMap1.rg - vec2(normalMapCenterRG)) * 2.0f, normalMap1.b));
    vec3 normalTextureSpace2 = normalize(vec3((normalMap2.rg - vec2(normalMapCenterRG)) * 2.0f, normalMap2.b));

    // calculate tangent-bitangent-normal matrix (TBN)
    vec3 normal = normalize(fsIn.normal);
    vec3 tangent = normalize(fsIn.tangent);
    vec3 bitangent = normalize(cross(normal, tangent));
    mat3 TBN = mat3(tangent, bitangent, normal);

    // calculate normal in view space
    vec3 normalViewSpace1 = TBN * normalTextureSpace1;
    vec3 normalViewSpace2 = TBN * normalTextureSpace2;
    if(!gl_FrontFacing){
        normalViewSpace1 = -normalViewSpace1;
        normalViewSpace2 = -normalViewSpace2;
    }
    vec3 waterNormal = normalize(mix(normal, normalViewSpace1, strengthDuDvNormal1.y) + mix(normal, normalViewSpace2, strengthDuDvNormal2.y));

    // projective texture mapping
    vec2 screenCoord = ((fsIn.clipSpacePosition.xy / fsIn.clipSpacePosition.w) + vec2(1.0f)) / 2.0f;
    vec2 texCoordReflection = vec2(screenCoord.x, 1.0f - screenCoord.y);
    vec2 texCoordRefraction = screenCoord;

    // calculate water depth in view direction
    float viewDepthToScene = texture(textureWaterRefraction, screenCoord).a;
    float viewDepthToWater = sqrt(dot(fsIn.position, fsIn.position));
    float waterDepth = viewDepthToScene - viewDepthToWater;

    // distorting the texture coordinates
    vec2 distortion = dudvMap1 * strengthDuDvNormal1.x + dudvMap2 * strengthDuDvNormal2.x;
    distortion *= clamp(waterDepth * distortionWaterDepthScale, 0.0f, 1.0f);

    // sample reflection and refraction color
    vec3 reflectionColor = texture(textureWaterReflection, texCoordReflection + distortion).rgb;
    vec3 refractionColor = texture(textureWaterRefraction, texCoordRefraction + distortion).rgb;

    // final water color
    vec3 viewDirection = normalize(fsIn.position);
    float refractionAmount = dot(waterNormal, -viewDirection);
    refractionAmount *= refractionAmount;
    vec3 waterColor = mix(reflectionColor * reflectionColorMultiplier, refractionColor * refractionColorMultiplier, refractionAmount);

    // set G-buffer outputs
    gBufferPosition = fsIn.position;
    gBufferDiffuse = waterColor;
    gBufferNormal = waterNormal;
    gBufferEmissionReflection = vec4(vec3(0.0f), 1.0f);
    gBufferSpecularShininess = vec4(specularColor, shininess);
    gBufferFaceNormal = normalize(fsIn.normal);
}


#endif /* FRAGMENT_SHADER */

