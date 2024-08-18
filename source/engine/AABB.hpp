#pragma once


#include <Common.hpp>


/**
 * @brief A three-dimensional axis-aligned bounding box.
 */
class AABB {
    public:
        glm::vec3 lowestPosition;
        glm::vec3 dimension;

        /**
         * @brief Construct a new AABB and set all values to zero.
         */
        AABB(){ Clear(); }

        /**
         * @brief Clear the AABB and set all values to zero.
         */
        void Clear(void){
            lowestPosition = glm::vec3(0.0);
            dimension = glm::vec3(0.0);
        }

        /**
         * @brief Transform this AABB by a given matrix.
         * @param[in] M The transformation matrix.
         * @details All corners of the current AABB are transformed and this AABB is updated to contain all transformed corner points.
         */
        void Transform(const glm::mat4& M){
            std::array<glm::vec4,8> points;
            points[0] = M * glm::vec4(lowestPosition, 1.0);
            points[1] = M * glm::vec4(lowestPosition + glm::vec3(dimension.x, 0.0f       , 0.0f       ), 1.0);
            points[2] = M * glm::vec4(lowestPosition + glm::vec3(0.0f       , dimension.y, 0.0f       ), 1.0);
            points[3] = M * glm::vec4(lowestPosition + glm::vec3(dimension.x, dimension.y, 0.0f       ), 1.0);
            points[4] = M * glm::vec4(lowestPosition + glm::vec3(0.0f       , 0.0f       , dimension.z), 1.0);
            points[5] = M * glm::vec4(lowestPosition + glm::vec3(dimension.x, 0.0f       , dimension.z), 1.0);
            points[6] = M * glm::vec4(lowestPosition + glm::vec3(0.0f       , dimension.y, dimension.z), 1.0);
            points[7] = M * glm::vec4(lowestPosition + glm::vec3(dimension.x, dimension.y, dimension.z), 1.0);
            glm::vec3 maxPosition = lowestPosition = glm::vec3(points[0]);
            for(size_t i = 1; i < points.size(); ++i){
                lowestPosition.x = std::min(lowestPosition.x, points[i].x);
                lowestPosition.y = std::min(lowestPosition.y, points[i].y);
                lowestPosition.z = std::min(lowestPosition.z, points[i].z);
                maxPosition.x = std::max(maxPosition.x, points[i].x);
                maxPosition.y = std::max(maxPosition.y, points[i].y);
                maxPosition.z = std::max(maxPosition.z, points[i].z);
            }
            dimension = maxPosition - lowestPosition;
        }
};

