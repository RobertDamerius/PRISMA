#pragma once


#include <Common.hpp>
#include <AABB.hpp>


/**
 * @brief Represents a point light.
 */
class PointLight {
    public:
        bool visible;               // True if the point light should be visible, false otherwise.
        glm::vec3 color;            // The color of the point light.
        glm::vec3 position;         // The world-space position of the point light.
        GLfloat quadraticFalloff;   // Coefficient for light attenuation according to 1 / (1 + c*x^2).
        GLfloat invRadiusPow4;      // Inverse of radius to the power of 4, where the radius represents the range of the light.

        /**
         * @brief Construct a new point light with default values.
         */
        PointLight(){ Reset(); }

        /**
         * @brief Reset this light to default values.
         * @details @ref color is set to zero, @ref quadraticFalloff and @ref invRadiusPow4 are set to one.
         */
        void Reset(void){
            visible = false;
            color = glm::vec3(0.0f);
            position = glm::vec3(0.0f);
            quadraticFalloff = 1.0f;
            invRadiusPow4 = 1.0f;
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

