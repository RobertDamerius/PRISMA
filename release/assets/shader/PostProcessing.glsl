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
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    texCoord = textureCoord;
}


#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FRAGMENT SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef FRAGMENT_SHADER
#include "PostProcessingDetails.glsl"


// fragment shader input
in vec2 texCoord;


// fragment shader output
layout (location = 0) out vec3 fragColor;


// uniforms
layout (binding = 0) uniform sampler2D textureGUI;
layout (binding = 1) uniform sampler2D textureEnvironmentAlbedo;
layout (binding = 2) uniform sampler2D textureBloom;
uniform float invGamma;
uniform float bloomStrength;
uniform float ditheringNoiseGranularity;
uniform float exposure;
uniform float toneMappingStrength;


// fragment shader main
void main(void){
    // calculate final scene color
    vec3 sceneColor = fxaaoe(textureEnvironmentAlbedo, texCoord);
    vec3 bloomColor = texture(textureBloom, texCoord).rgb;
    vec3 finalColor = sceneColor + bloomStrength * bloomColor;

    // exposure, tonemapping and gamma correction
    finalColor = mix(vec3(1.0f) - exp(-finalColor * exposure), finalColor, exposure < 0.0f);
    finalColor = ToneMapping(finalColor, toneMappingStrength);
    finalColor = pow(finalColor, vec3(invGamma));

    // mix in GUI color
    vec4 guiColor = texture(textureGUI, texCoord);
    fragColor = mix(finalColor, guiColor.rgb, guiColor.a);

    // dithering
    vec2 texelSize = 1.0f / textureSize(textureEnvironmentAlbedo, 0);
    fragColor += DitheringNoise(texelSize, ditheringNoiseGranularity);

    // for debugging
    // fragColor = texture(textureGUI, texCoord).rgb;
    // fragColor = texture(textureEnvironmentAlbedo, texCoord).rgb;
    // fragColor = texture(textureBloom, texCoord).rgb;
}


#endif /* FRAGMENT_SHADER */

