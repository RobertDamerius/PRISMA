#pragma once


#include <Common.hpp>


namespace VectorUtils {


/**
 * @brief Do a safe normalization of the three-dimensional vector.
 * @param[in] v The vector to be normalized.
 * @param[in] fallback A fallback value to be set, if the length of v is close to zero.
 * @return Normalized vector of v or the fallback value, if the length of v is close to zero.
 */
inline glm::vec3 Normalize(glm::vec3 v, glm::vec3 fallback = glm::vec3(1.0f, 0.0f, 0.0f)){
    GLfloat L = std::sqrt(glm::dot(v,v));
    if(L > std::numeric_limits<double>::epsilon()){
        return v / L;
    }
    return fallback;
}


} /* namespace: VectorUtils */

