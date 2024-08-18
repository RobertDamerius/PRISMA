// uniforms
uniform float shadowCascadeFarPlanes[$NUMBER_OF_SHADOW_CASCADES$];   // Far planes for all cascades (the last value is equal to the far clipping plane of the view camera).
uniform vec2 shadowBiasMinMax;                                       // Minimum and maximum value for shadow bias.


/**
 * @brief Calculate the amount of shadow for the current fragment position.
 * @param[in] shadowMap The depth textures for all shadow cascades.
 * @param[in] fragPosViewSpace Fragment position in view-space.
 * @param[in] faceNormal Face normal in view-space.
 * @param[in] lightDirection Normalized light direction in view-space.
 * @return A scalar indicating the amount of shadow. 0: no shadow, 1: shadow.
 */
float ShadowCalculation(sampler2DArray shadowMap, vec3 fragPosViewSpace, vec3 faceNormal, vec3 lightDirection){
    // select cascade layer index depending on the fragments z position (view space)
    const int maxLayerIndex = $NUMBER_OF_SHADOW_CASCADES$ - 1;
    int layer = maxLayerIndex;
    for(int i = 0; i < maxLayerIndex; ++i){
        if(-fragPosViewSpace.z < shadowCascadeFarPlanes[i]){
            layer = i;
            break;
        }
    }

    // transform fragment position to light-space
    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosViewSpace, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // projCoords are now in rage [0,1] like texture coords for the depth shadow texture

    // get depth of current fragment from lights perspective (normalized [0,1])
    float currentDepth = projCoords.z;

    // keep shadow at 0.0 if outside the far plane of the lights frustum
    if(currentDepth > 1.0f){
        return 0.0f;
    }

    // bias to reduce artefacts (shadow acne)
    float fn_dot_l = dot(faceNormal, lightDirection);
    float bias = max(shadowBiasMinMax.x, shadowBiasMinMax.y * (1.0 + fn_dot_l));

    // percentage-closer filtering (3-by-3)
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for(int x = -1; x <= 1; ++x){
        for(int y = -1; y <= 1; ++y){
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r; // texture contains normalized depth [0,1]
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

