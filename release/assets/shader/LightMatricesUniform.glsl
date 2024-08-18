// light matrices uniform block
layout (std140, binding = $UBO_LIGHTMATRICES$) uniform LightSpaceMatrices {
    mat4 lightSpaceMatrices[$NUMBER_OF_SHADOW_CASCADES$];   // projection-view-invCameraView matrix for each shadow cascade.
};

