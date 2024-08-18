#pragma once


#include <Common.hpp>


/**
 * @brief Represents the properties of a dynamic mesh.
 */
class DynamicMeshProperties {
    public:
        bool visible;                        // True if this mesh object is visible, false otherwise.
        bool castShadow;                     // True if this mesh casts a shadow, false otherwise.
        glm::vec3 position;                  // Position in OpenGL world space coordinates.
        glm::vec4 quaternion;                // Unit quaternion that represents the orientation of this mesh.
        glm::vec3 scale;                     // Scaling of this mesh object in body frame coordinates.
        glm::vec3 diffuseColorMultiplier;    // Diffuse color multiplier for this mesh.
        glm::vec3 specularColorMultiplier;   // Specular color multiplier for this mesh.
        glm::vec3 emissionColorMultiplier;   // Emission color multiplier for this mesh.
        GLfloat shininessMultiplier;         // Shininess multiplier for this mesh.

        /**
         * @brief Construct a new dynamic mesh property object.
         */
        DynamicMeshProperties(){
            visible = false;
            castShadow = false;
            position = glm::vec3(0.0f);
            quaternion = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            scale = glm::vec3(1.0f);
            diffuseColorMultiplier = glm::vec3(1.0f);
            specularColorMultiplier = glm::vec3(1.0f);
            emissionColorMultiplier = glm::vec3(1.0f);
            shininessMultiplier = 1.0f;
        }

        /**
         * @brief Get the model matrix based on @ref scale, @ref quaternion and @ref position.
         * @return The model matrix containing scale, rotation and translation.
         */
        glm::mat4 GetModelMatrix(void) const {
            GLfloat q0q0 = quaternion.w * quaternion.w;
            GLfloat q1q1 = quaternion.x * quaternion.x;
            GLfloat q2q2 = quaternion.y * quaternion.y;
            GLfloat q3q3 = quaternion.z * quaternion.z;
            GLfloat q1q2 = quaternion.x * quaternion.y;
            GLfloat q0q3 = quaternion.w * quaternion.z;
            GLfloat q1q3 = quaternion.x * quaternion.z;
            GLfloat q0q2 = quaternion.w * quaternion.y;
            GLfloat q2q3 = quaternion.y * quaternion.z;
            GLfloat q0q1 = quaternion.w * quaternion.x;
            glm::mat4 Cb2n(1.0f); // column-major order, elements stored as mat[col][row]
            Cb2n[0][0] = q0q0+q1q1-q2q2-q3q3;
            Cb2n[0][1] = q1q2+q1q2+q0q3+q0q3;
            Cb2n[0][2] = q1q3+q1q3-q0q2-q0q2;
            Cb2n[1][0] = q1q2+q1q2-q0q3-q0q3;
            Cb2n[1][1] = q0q0-q1q1+q2q2-q3q3;
            Cb2n[1][2] = q2q3+q2q3+q0q1+q0q1;
            Cb2n[2][0] = q1q3+q1q3+q0q2+q0q2;
            Cb2n[2][1] = q2q3+q2q3-q0q1-q0q1;
            Cb2n[2][2] = q0q0-q1q1-q2q2+q3q3;
            glm::mat4 modelMatrix(1.0f);
            modelMatrix = glm::translate(modelMatrix, position);
            modelMatrix = modelMatrix * Cb2n;
            modelMatrix = glm::scale(modelMatrix, scale);
            return modelMatrix;
        }
};

