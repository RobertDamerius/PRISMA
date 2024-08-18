/**
 * @brief Generate dithering noise color.
 * @param[in] texelSize Size of a 2D texel.
 * @param[in] ditheringNoiseGranularity Amount of dithering noise, e.g. 0.5 / 255.0.
 * @return Dithering noise color.
 */
vec3 DitheringNoise(vec2 texelSize, float ditheringNoiseGranularity){
    vec2 coordinates = gl_FragCoord.xy * texelSize;
    float random = fract(sin(dot(coordinates.xy, vec2(12.9898,78.233))) * 43758.5453);
    float f = mix(-ditheringNoiseGranularity, ditheringNoiseGranularity, random);
    return vec3(f);
}


/**
 * @brief FXAA with outlier elimination.
 * @param[in] inputTexture The input texture sampler (2D).
 * @param[in] texCoords The texture coordinates.
 * @return FXAA applied on input texture.
 */
vec3 fxaaoe(sampler2D inputTexture, vec2 texCoords){
    const vec3 luma = vec3(0.299f, 0.587f, 0.114f);
    vec2 pixelSize = 1.0f / textureSize(inputTexture, 0);
    vec3 TL = texture(inputTexture, texCoords - pixelSize).rgb;
    vec3 TR = texture(inputTexture, texCoords - vec2(-1.0f, 1.0f) * pixelSize).rgb;
    vec3 BR = texture(inputTexture, texCoords + pixelSize).rgb;
    vec3 BL = texture(inputTexture, texCoords + vec2(-1.0f, 1.0f) * pixelSize).rgb;
    vec3 M = texture(inputTexture, texCoords).rgb;
    vec3 cornerSum = TL + TR + BR + BL;
    vec3 averageFive = 0.2f * (cornerSum + M);
    vec3 dTL = TL - averageFive;
    vec3 dTR = TR - averageFive;
    vec3 dBR = BR - averageFive;
    vec3 dBL = BL - averageFive;
    vec3 dM = M - averageFive;
    if(dot(dM, dM) > dot(dTL, dTL) + dot(dTR, dTR) + dot(dBR, dBR) + dot(dBL, dBL) + 0.1f){
        return 0.25f * cornerSum;
    }
    float lumaTL = dot(luma, TL);
    float lumaTR = dot(luma, TR);
    float lumaBR = dot(luma, BR);
    float lumaBL = dot(luma, BL);
    float lumaM  = dot(luma, M);
    vec2 dir = vec2(lumaBL + lumaBR - lumaTL - lumaTR, lumaTL + lumaBL - lumaTR - lumaBR);
    dir = min(vec2(8.0f), max(vec2(-8.0f), dir / (min(abs(dir.x), abs(dir.y)) + max((lumaTL + lumaTR + lumaBL + lumaBR) * 0.03125f, 0.0078125f)))) * pixelSize;
    vec3 result1 = 0.5f * (texture(inputTexture, texCoords - (1.0f / 18.0f) * dir).xyz + texture(inputTexture, texCoords + (1.0f / 18.0f) * dir).xyz);
    vec3 result2 = result1 * 0.5f + 0.25f * (texture(inputTexture, texCoords - (1.0f / 6.0f) * dir).xyz + texture(inputTexture, texCoords + (1.0f / 6.0f) * dir).xyz);
    float lumaResult2 = dot(luma, result2);
    return mix(result2, result1, float(lumaResult2 < min(lumaM, min(lumaTL, min(lumaTR, min(lumaBL, lumaBR)))) || lumaResult2 > max(lumaM, max(lumaTL, max(lumaTR, max(lumaBL, lumaBR))))));
}


/**
 * @brief The tonemapping function to be used.
 * @param[in] x The input color.
 * @return Output color.
 */
vec3 ToneMapping(vec3 x, float amount){
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    vec3 aces = clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f); // ACES filmic tonemapping
    return mix(x, aces, amount);
}

