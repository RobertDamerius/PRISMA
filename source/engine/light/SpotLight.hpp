#pragma once


#include <Common.hpp>
#include <AABB.hpp>


/**
 * @brief Represents a spot light.
 */
class SpotLight {
    public:
        bool visible;                  // True if the spot light should be visible, false otherwise.
        glm::vec3 color;               // Color of the spot light.
        glm::vec3 position;            // World-space position of the spot light.
        glm::vec3 direction;           // Normalized direction vector of the spot light.
        GLfloat quadraticFalloff;      // Coefficient for light attenuation according to 1 / (1 + c*x^2).
        GLfloat invRadiusPow4;         // Inverse of radius to the power of 4, where the radius represents the range of the light.
        GLfloat cosInnerCutOffAngle;   // Cosine of the inner cut-off angle of light cone.
        GLfloat cosOuterCutOffAngle;   // Cosine of the outer cut-off angle of light cone.

        /**
         * @brief Construct a new spot light with default values.
         */
        SpotLight(){ Reset(); }

        /**
         * @brief Reset this light to default values.
         * @details @ref color, @ref innerCutoffAngle and @ref outerCutoffAngle are set to zero, @ref direction is set to (0,-1,0), @ref quadraticFalloff and @ref invRadiusPow4 are set to one.
         */
        void Reset(void){
            visible = false;
            color = glm::vec3(0.0f);
            position = glm::vec3(0.0f);
            direction = glm::vec3(0.0f, -1.0f, 0.0f);
            quadraticFalloff = 1.0f;
            invRadiusPow4 = 1.0f;
            cosInnerCutOffAngle = 1.0f;
            cosOuterCutOffAngle = 1.0f;
        }

        /**
         * @brief Calculate the axis-aligned bounding box.
         * @return The AABB containing the box.
         */
        AABB CalculateAABB(void) const {
            AABB aabb;
            double R = std::sqrt(std::sqrt(1.0 / invRadiusPow4));
            aabb.lowestPosition = position - glm::vec3(R);
            aabb.dimension = glm::vec3(R + R);
            return aabb;
        }
};

