#pragma once


#include <Common.hpp>


/**
 * @brief Represents an ambient light.
 */
class AmbientLight {
    public:
        glm::vec3 color;   // Color of the ambient light.

        /**
         * @brief Construct a new ambient light with default values.
         */
        AmbientLight(){ Clear(); }

        /**
         * @brief Clear this light and set default values.
         * @details @ref color is set to zero.
         */
        void Clear(void){
            color = glm::vec3(0.0f);
        }
};

