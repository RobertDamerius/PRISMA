// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERTEX SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef VERTEX_SHADER


// vertex shader input
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 textureCoord;


// vertex shader output
out vec2 texCoord;


// uniforms
uniform vec4 imageTransform;


// vertex shader main
void main(void){
    gl_Position = vec4(imageTransform.xy + imageTransform.zw*position, 0.0f, 1.0f);
    texCoord = textureCoord;
}


#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FRAGMENT SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef FRAGMENT_SHADER


// fragment shader input
in vec2 texCoord;


// fragment shader output
layout (location = 0) out vec4 fragColor;


// uniforms
layout (binding = 0) uniform sampler2D textureImage;
uniform vec4 imageColorScale;


// fragment shader main
void main(){
    vec4 imageColor = texture(textureImage, texCoord);
    fragColor = imageColorScale * imageColor;
}


#endif /* FRAGMENT_SHADER */

