/**
 * @brief G-buffer structure definition
 */
struct GBuffer {
    vec3 position;               // View space position.
    vec3 normal;                 // View space normal vector.
    vec3 color;                  // Object color in linear RGB space.
    vec3 emission;               // Emission color.
    vec3 specular;               // Specular color.
    float shininess;             // Shininess coefficient.
    vec3 faceNormal;             // View space face normal vector.
    float reflectionIndicator;   // Reflection indicator (> 0.5 means it's a reflective texture).
};


/**
 * @brief Get the G-buffer structure.
 * @param[in] texPosition The 2D sampler containing position.
 * @param[in] texNormal The 2D sampler containing the normal vector.
 * @param[in] texDiffuse The 2D sampler containing the diffuse color.
 * @param[in] texEmission The 2D sampler containing the emission color.
 * @param[in] texSpecularShininess The 2D sampler containing the specular color (RGB) and shininess coefficient (A).
 * @param[in] texCoord The current texture coordinates for this fragment.
 * @return The G-buffer structure.
 */
GBuffer GetGBuffer(sampler2D texPosition, sampler2D texNormal, sampler2D texDiffuse, sampler2D texEmission, sampler2D texSpecularShininess, sampler2D texFaceNormal, vec2 texCoord){
    const float minShininess = 1.0f;
    GBuffer gBuffer;
    gBuffer.position = texture(texPosition, texCoord).rgb;
    gBuffer.normal = texture(texNormal, texCoord).rgb;
    gBuffer.color = texture(texDiffuse, texCoord).rgb;
    vec4 emissionReflection = texture(texEmission, texCoord);
    gBuffer.emission = emissionReflection.rgb;
    vec4 specularShininess = texture(texSpecularShininess, texCoord);
    gBuffer.specular = specularShininess.rgb;
    gBuffer.shininess = max(minShininess, specularShininess.a);
    gBuffer.faceNormal = texture(texFaceNormal, texCoord).rgb;
    gBuffer.reflectionIndicator = emissionReflection.a;
    return gBuffer;
}

