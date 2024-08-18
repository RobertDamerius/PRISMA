/**
 * @brief Data structure for point lights stored in a shader storage buffer object.
 */
struct DataDirectionalLight {
    float r;                  // Red color component of the directional light color.
    float g;                  // Green color component of the directional light color.
    float b;                  // Blue color component of the directional light color.
    float nx;                 // x direction of the directional light direction starting from the light source (view-space). vec3(nx, ny, nz) should be normalized.
    float ny;                 // y direction of the directional light direction starting from the light source (view-space). vec3(nx, ny, nz) should be normalized.
    float nz;                 // z direction of the directional light direction starting from the light source (view-space). vec3(nx, ny, nz) should be normalized.
};

struct DataPointLight {
    float r;                  // Red color component of the point light color.
    float g;                  // Green color component of the point light color.
    float b;                  // Blue color component of the point light color.
    float x;                  // x position of the point light (view-space).
    float y;                  // y position of the point light (view-space).
    float z;                  // z position of the point light (view-space).
    float invR4;              // 1/(R^4) with R being the radius of the point light.
    float quadraticFalloff;   // Quadratic fall-off for light attenuation.
};

struct DataSpotLight {
    float r;                  // Red color component of the spot light color.
    float g;                  // Green color component of the spot light color.
    float b;                  // Blue color component of the spot light color.
    float x;                  // x position of the spot light (view-space).
    float y;                  // y position of the spot light (view-space).
    float z;                  // z position of the spot light (view-space).
    float nx;                 // x direction of the spot light direction starting from the light source (view-space). vec3(nx, ny, nz) should be normalized.
    float ny;                 // y direction of the spot light direction starting from the light source (view-space). vec3(nx, ny, nz) should be normalized.
    float nz;                 // z direction of the spot light direction starting from the light source (view-space). vec3(nx, ny, nz) should be normalized.
    float invR4;              // 1/(R^4) with R being the radius of the spot light.
    float quadraticFalloff;   // Quadratic fall-off for light attenuation.
    float cosInnerCutOff;     // cos(a1) where a1 is the inner angle where the smooth cut-off of the spot light begins.
    float cosOuterCutOff;     // cos(a2) where a2 is the outer angle where the smooth cut-off of the spot light ends.
};


// uniforms
uniform vec3 ambientLightColor;
uniform uint numDirectionalLights;
uniform uint numPointLights;
uniform uint numSpotLights;


// shader storage buffer objects (SSBO) for lights casters
layout(std430, binding = $SSBO_DIRECTIONALLIGHTS$) readonly buffer SSBODirectionalLights {
    DataDirectionalLight directionalLights[$MAX_NUMBER_DIRECTIONALLIGHTS$];
};

layout(std430, binding = $SSBO_POINTLIGHTS$) readonly buffer SSBOPointLights {
    DataPointLight pointLights[$MAX_NUMBER_POINTLIGHTS$];
};

layout(std430, binding = $SSBO_SPOTLIGHTS$) readonly buffer SSBOSpotLights {
    DataSpotLight spotLights[$MAX_NUMBER_SPOTLIGHTS$];
};


/**
 * @brief Calculate the current fragment color for a given directional light.
 * @param[in] object The G-buffer textures.
 * @param[in] viewDirection The normalized camera view direction from camera to fragment position, given in view space.
 * @param[in] lightColor Color of the directional light source.
 * @param[in] lightDirection Normalized direction vector from the light source to the fragment, given in view space.
 * @return The fragment color based on the directional light.
 */
vec3 DirectionalLight(GBuffer object, vec3 viewDirection, vec3 lightColor, vec3 lightDirection){
    float diffuseFactor = max(-dot(object.normal, lightDirection), 0.0f);
    float specularFactor = pow(max(-dot(viewDirection, reflect(lightDirection, object.normal)), 0.0f), object.shininess);
    return (diffuseFactor * object.color + specularFactor * object.specular) * lightColor;
}

/**
 * @brief Calculate the current fragment color for a given point light.
 * @param[in] object The G-buffer textures.
 * @param[in] viewDirection The normalized camera view direction from camera to fragment position, given in view space.
 * @param[in] lightColor Color of the point light source.
 * @param[in] lightPosition The light position, given in view space.
 * @param[in] invR4 1/(R^4) with R being the radius of the light source.
 * @param[in] quadraticFalloff Quadratic fall-off for light attenuation.
 * @return The fragment color based on the point light.
 */
vec3 PointLight(GBuffer object, vec3 viewDirection, vec3 lightColor, vec3 lightPosition, float invR4, float quadraticFalloff){
    vec3 lightDirection = object.position - lightPosition;
    float squaredDistance = dot(lightDirection, lightDirection);
    lightDirection /= (float(squaredDistance < 0.00001) + sqrt(squaredDistance));
    float diffuseFactor = max(-dot(object.normal, lightDirection), 0.0f);
    float specularFactor = pow(max(-dot(viewDirection, reflect(lightDirection, object.normal)), 0.0f), object.shininess);
    float attenuation = max((1.0f - invR4 * squaredDistance * squaredDistance) / (1.0f + quadraticFalloff * squaredDistance), 0.0f);
    return attenuation * (diffuseFactor * object.color + specularFactor * object.specular) * lightColor;
}

/**
 * @brief Calculate the current fragment color for a given spot light.
 * @param[in] object The G-buffer textures.
 * @param[in] viewDirection The normalized camera view direction from camera to fragment position, given in view space.
 * @param[in] lightColor Color of the spot light source.
 * @param[in] lightPosition The light position, given in view space.
 * @param[in] invR4 1/(R^4) with R being the radius of the light source.
 * @param[in] quadraticFalloff Quadratic fall-off for light attenuation.
 * @param[in] spotDirection Normalized direction of the spot light from the light source to the fragment, given in view space.
 * @return The fragment color based on the spot light.
 */
vec3 SpotLight(GBuffer object, vec3 viewDirection, vec3 lightColor, vec3 lightPosition, float invR4, float quadraticFalloff, vec3 spotDirection, float cosInnerCutOff, float cosOuterCutOff){
    vec3 lightDirection = object.position - lightPosition;
    float squaredDistance = dot(lightDirection, lightDirection);
    lightDirection /= (float(squaredDistance < 0.00001) + sqrt(squaredDistance));
    float diffuseFactor = max(-dot(object.normal, lightDirection), 0.0f);
    float specularFactor = pow(max(-dot(viewDirection, reflect(lightDirection, object.normal)), 0.0f), object.shininess);
    float attenuation = max((1.0f - invR4 * squaredDistance * squaredDistance) / (1.0f + quadraticFalloff * squaredDistance), 0.0f);
    float theta = dot(spotDirection,lightDirection);
    float epsilon = cosInnerCutOff - cosOuterCutOff;
    float spotFactor = clamp((dot(spotDirection,lightDirection) - cosOuterCutOff) / (float(epsilon < 0.00001) + epsilon), 0.0, 1.0);
    return (attenuation * spotFactor) * (diffuseFactor * object.color + specularFactor * object.specular) * lightColor;
}

/**
 * @brief Calculate the current fragment color for all directional lights.
 * @param[in] object The G-buffer textures.
 * @param[in] viewDirection The normalized camera view direction from camera to fragment position, given in view space.
 * @return The fragment color based on the directional lights.
 */
vec3 AllDirectionalLights(GBuffer object, vec3 viewDirection){
    vec3 result = vec3(0.0f);
    for(uint i = 0; i < numDirectionalLights; ++i){
        vec3 lightColor = vec3(directionalLights[i].r, directionalLights[i].g, directionalLights[i].b);
        vec3 lightDirection = vec3(directionalLights[i].nx, directionalLights[i].ny, directionalLights[i].nz);
        result += DirectionalLight(object, viewDirection, lightColor, lightDirection);
    }
    return result;
}

/**
 * @brief Calculate the current fragment color for all point lights.
 * @param[in] object The G-buffer textures.
 * @param[in] viewDirection The normalized camera view direction from camera to fragment position, given in view space.
 * @return The fragment color based on the point lights.
 */
vec3 AllPointLights(GBuffer object, vec3 viewDirection){
    vec3 result = vec3(0.0f);
    for(uint i = 0; i < numPointLights; ++i){
        vec3 lightColor = vec3(pointLights[i].r, pointLights[i].g, pointLights[i].b);
        vec3 lightPosition = vec3(pointLights[i].x, pointLights[i].y, pointLights[i].z);
        result += PointLight(object, viewDirection, lightColor, lightPosition, pointLights[i].invR4, pointLights[i].quadraticFalloff);
    }
    return result;
}

/**
 * @brief Calculate the current fragment color for all splot lights.
 * @param[in] object The G-buffer textures.
 * @param[in] viewDirection The normalized camera view direction from camera to fragment position, given in view space.
 * @return The fragment color based on the splot lights.
 */
vec3 AllSpotLights(GBuffer object, vec3 viewDirection){
    vec3 result = vec3(0.0f);
    for(uint i = 0; i < numSpotLights; ++i){
        vec3 lightColor = vec3(spotLights[i].r, spotLights[i].g, spotLights[i].b);
        vec3 lightPosition = vec3(spotLights[i].x, spotLights[i].y, spotLights[i].z);
        vec3 lightSpotDirection = vec3(spotLights[i].nx, spotLights[i].ny, spotLights[i].nz);
        result += SpotLight(object, viewDirection, lightColor, lightPosition, spotLights[i].invR4, spotLights[i].quadraticFalloff, lightSpotDirection, spotLights[i].cosInnerCutOff, spotLights[i].cosOuterCutOff);
    }
    return result;
}

