#pragma once


#include <Common.hpp>


class WaterMeshParameter {
    public:
        bool visible;                                // True if the water mesh is visible, false otherwise.
        glm::vec3 position;                          // World space position of the water mesh.
        GLfloat textureCoordinateScaling1;           // Scaling (1) for texture coordinates of the texture maps.
        GLfloat textureCoordinateScaling2;           // Scaling (2) for texture coordinates of the texture maps.
        glm::vec2 textureCoordinateAnimation1;       // Texture coordinate animation time (1) for the texture maps.
        glm::vec2 textureCoordinateAnimation2;       // Texture coordinate animation time (2) for the texture maps.
        glm::vec3 specularColor;                     // The specular color of the water mesh.
        GLfloat shininess;                           // The shininess value of the water mesh.
        glm::vec3 reflectionColorMultiplier;         // Color multiplier for the reflection texture.
        glm::vec3 refractionColorMultiplier;         // Color multiplier for the refraction texture.
        GLfloat clippingPlaneOffset;                 // Offset for the clipping plane separating reflection and refraction.
        glm::vec2 strengthDuDvNormal1;               // Strength (1) for the distortion du/dv-texture map and for the normal texture map of the water mesh.
        glm::vec2 strengthDuDvNormal2;               // Strength (2) for the distortion du/dv-texture map and for the normal texture map of the water mesh.
        GLfloat distortionWaterDepthScale;           // Multiply distortion with the scaled water depth to reduce artefacts near the water edge.

        /**
         * @brief Construct a new water mesh parameter object and set default values.
         */
        WaterMeshParameter(){ Clear(); }

        /**
         * @brief Clear all attributes to default values.
         */
        void Clear(void){
            visible = true;
            position = glm::vec3(0.0f);
            textureCoordinateScaling1 = 1.0;
            textureCoordinateScaling2 = 1.0;
            textureCoordinateAnimation1 = glm::vec2(0.0f);
            textureCoordinateAnimation2 = glm::vec2(0.0f);
            specularColor = glm::vec3(1.0f);
            shininess = 64.0;
            reflectionColorMultiplier = glm::vec3(1.0f);
            refractionColorMultiplier = glm::vec3(0.1f, 0.19f, 0.22f);
            clippingPlaneOffset = 0.01;
            strengthDuDvNormal1 = glm::vec2(0.01, 0.1);
            strengthDuDvNormal2 = glm::vec2(0.01, 0.1);
            distortionWaterDepthScale = 0.2f;
        }
};

