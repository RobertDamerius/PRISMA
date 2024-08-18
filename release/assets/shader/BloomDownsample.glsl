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
layout (location = 0) out vec3 downsample;


// uniforms
layout (binding = 0) uniform sampler2D inputTexture;
uniform vec2 texelSize;
uniform int mipLevel;


// helper function for karis average
float KarisAverage(vec3 linearRGB){
    const vec3 invGamma = vec3(1.0f / 2.2f);
    vec3 sRGB = pow(linearRGB, invGamma);
    float luma = dot(sRGB, vec3(0.2126f, 0.7152f, 0.0722f));
    return 1.0f / (1.0f + luma*0.25f);
}


// fragment shader main
void main(){
    // take 13 samples around current texel (e)
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    vec4 s = texelSize.xyxy * vec4(1.0f, 1.0f, 2.0f, 2.0f);
    vec3 a = texture(inputTexture, vec2(texCoord.x - s.z, texCoord.y + s.w)).rgb;
    vec3 b = texture(inputTexture, vec2(texCoord.x,       texCoord.y + s.w)).rgb;
    vec3 c = texture(inputTexture, vec2(texCoord.x + s.z, texCoord.y + s.w)).rgb;
    vec3 d = texture(inputTexture, vec2(texCoord.x - s.z, texCoord.y      )).rgb;
    vec3 e = texture(inputTexture, vec2(texCoord.x,       texCoord.y      )).rgb;
    vec3 f = texture(inputTexture, vec2(texCoord.x + s.z, texCoord.y      )).rgb;
    vec3 g = texture(inputTexture, vec2(texCoord.x - s.z, texCoord.y - s.w)).rgb;
    vec3 h = texture(inputTexture, vec2(texCoord.x,       texCoord.y - s.w)).rgb;
    vec3 i = texture(inputTexture, vec2(texCoord.x + s.z, texCoord.y - s.w)).rgb;
    vec3 j = texture(inputTexture, vec2(texCoord.x - s.x, texCoord.y + s.y)).rgb;
    vec3 k = texture(inputTexture, vec2(texCoord.x + s.x, texCoord.y + s.y)).rgb;
    vec3 l = texture(inputTexture, vec2(texCoord.x - s.x, texCoord.y - s.y)).rgb;
    vec3 m = texture(inputTexture, vec2(texCoord.x + s.x, texCoord.y - s.y)).rgb;

    // combine all texels to the final downsampled image
    vec3 groups[5];
    switch(mipLevel){
        case 0:
            // first mip level: apply Karis average to each block of 4 samples to prevent fireflies (very bright subpixels, leads to pulsating artifacts)
            groups[0] = (a+b+d+e) * (0.125f/4.0f);
            groups[1] = (b+c+e+f) * (0.125f/4.0f);
            groups[2] = (d+e+g+h) * (0.125f/4.0f);
            groups[3] = (e+f+h+i) * (0.125f/4.0f);
            groups[4] = (j+k+l+m) * (0.5f/4.0f);
            groups[0] *= KarisAverage(groups[0]);
            groups[1] *= KarisAverage(groups[1]);
            groups[2] *= KarisAverage(groups[2]);
            groups[3] *= KarisAverage(groups[3]);
            groups[4] *= KarisAverage(groups[4]);
            downsample = groups[0] + groups[1] + groups[2] + groups[3] + groups[4];
            downsample = max(downsample, 0.0001f);
            break;
        default:
            // weighted distribution over all five blocks: (j,k,l,m)*0.5 + (a,b,d,e)*0.125 + (b,c,e,f)*0.125 + (d,e,g,h)*0.125 + (e,f,h,i)*0.125
            downsample = (e+j+k+l+m)*0.125f + (a+c+g+i)*0.03125f + (b+d+f+h)*0.0625f;
            break;
    }
}


#endif /* FRAGMENT_SHADER */

