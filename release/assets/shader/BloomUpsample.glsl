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


// fragment shader input
in vec2 texCoord;


// fragment shader output
layout (location = 0) out vec3 upsample;


// uniforms
layout (binding = 0) uniform sampler2D inputTexture;


// fragment shader main
void main(){
    // take 9 samples around current texel (e)
    // a - b - c
    // d - e - f
    // g - h - i
    vec2 texelSize = 1.0 / vec2(textureSize(inputTexture, 0));
    vec3 a = texture(inputTexture, vec2(texCoord.x - texelSize.x, texCoord.y + texelSize.y)).rgb;
    vec3 b = texture(inputTexture, vec2(texCoord.x              , texCoord.y + texelSize.y)).rgb;
    vec3 c = texture(inputTexture, vec2(texCoord.x + texelSize.x, texCoord.y + texelSize.y)).rgb;
    vec3 d = texture(inputTexture, vec2(texCoord.x - texelSize.x, texCoord.y              )).rgb;
    vec3 e = texture(inputTexture, vec2(texCoord.x              , texCoord.y              )).rgb;
    vec3 f = texture(inputTexture, vec2(texCoord.x + texelSize.x, texCoord.y              )).rgb;
    vec3 g = texture(inputTexture, vec2(texCoord.x - texelSize.x, texCoord.y - texelSize.y)).rgb;
    vec3 h = texture(inputTexture, vec2(texCoord.x              , texCoord.y - texelSize.y)).rgb;
    vec3 i = texture(inputTexture, vec2(texCoord.x + texelSize.x, texCoord.y - texelSize.y)).rgb;

    // apply weighted distribution (3x3 tent filter)
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    upsample = e*0.25f + (b+d+f+h)*0.125f + (a+c+g+i)*0.0625f;
}


#endif /* FRAGMENT_SHADER */

