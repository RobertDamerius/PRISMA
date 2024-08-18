// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERTEX SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef VERTEX_SHADER


// vertex shader input
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 textureCoord;


// vertex shader output
out vec2 texCoord;


// vertex shader main
void main(void){
    texCoord = textureCoord;
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
}


#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FRAGMENT SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef FRAGMENT_SHADER
#include "CameraUniform.glsl"


// fragment shader input
in vec2 texCoord;


// fragment shader output
layout (location = 0) out float occlusion;


// uniforms
layout (binding = 0) uniform sampler2D texturePosition;
layout (binding = 1) uniform sampler2D textureNormal;
layout (binding = 2) uniform sampler2D textureNoise;
uniform vec3 samples[$SSAO_KERNEL_SIZE$];
uniform uint kernelSize;
uniform vec2 noiseScale; // resolution of the screen divided by the size of the noise texture, e.g. (1920/4, 1080/4)
uniform float occlusionRadius;
uniform float occlusionBias;


// fragment shader main
void main(){
    // get input for SSAO algorithm
    vec3 fragPos = texture(texturePosition, texCoord).xyz;
    vec3 normal = normalize(texture(textureNormal, texCoord).rgb);
    vec3 randomVector = normalize(texture(textureNoise, texCoord * noiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    occlusion = 0.0;
    for(uint i = 0; i < kernelSize; ++i){
        // get sample position
        vec3 samplePosition = TBN * samples[i]; // from tangent to view-space
        samplePosition = fragPos + samplePosition * occlusionRadius; 

        // project sample position to sample texture (to get position on screen/texture)
        vec4 offset = vec4(samplePosition, 1.0);
        offset = cameraProjectionMatrix * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float sampleDepth = texture(texturePosition, offset.xy).z;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, occlusionRadius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePosition.z + occlusionBias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
}


#endif /* FRAGMENT_SHADER */

