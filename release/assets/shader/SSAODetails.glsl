/**
 * @brief Perform blurring on the SSAO texture.
 * @param[in] ssaoTexture The one-dimensional input texture to be blurred.
 * @param[in] texCoord The current texture coordinates of the framebuffer.
 * @return Blurred texture value.
 */
float SSAOBlur(sampler2D ssaoTexture, vec2 texCoord){
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for(int x = -2; x < 2; ++x){
        for(int y = -2; y < 2; ++y){
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, texCoord + offset).r;
        }
    }
    return result / (4.0 * 4.0);
}

