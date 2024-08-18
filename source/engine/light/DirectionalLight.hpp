#pragma once


#include <Common.hpp>


/**
 * @brief Represents a directional light.
 */
class DirectionalLight {
    public:
        bool visible;             // True if the directional light should be visible, false otherwise.
        glm::vec3 color;          // Color of the directional light.
        glm::vec3 direction;      // Normalized direction vector of the directional light.

        /**
         * @brief Construct a new directional light with default values.
         */
        DirectionalLight(){ Reset(); }

        /**
         * @brief Reset this light to default values.
         * @details @ref color and @ref ambient are set to zero and @ref direction is set to (0,-1,0).
         */
        void Reset(void){
            visible = false;
            color = glm::vec3(0.0f);
            direction = glm::vec3(0.0f, -1.0f, 0.0f);
        }
};

