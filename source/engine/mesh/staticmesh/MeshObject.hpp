#pragma once


#include <Common.hpp>
#include <StringHelper.hpp>


/**
 * @brief This enum represents the type of a mesh.
 */
enum MeshType {
    MESH_TYPE_INVALID,      // The mesh type is invalid.
    MESH_TYPE_COLORMESH,    // The mesh represents a color mesh.
    MESH_TYPE_TEXTUREMESH   // The mesh represents a texture mesh.
};


/**
 * @brief The mesh object represents a mesh to be rendered to the scene.
 */
class MeshObject {
    public:
        MeshType type;                       // The type of the mesh. This value is set by @ref SetType according the @ref name. The default value is MESH_TYPE_INVALID.
        std::string name;                    // The mesh name that indicates the mesh data.
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
         * @brief Construct a new mesh object.
         */
        MeshObject(){
            type = MESH_TYPE_INVALID;
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
         * @brief Set the type of this mesh based on the @ref name.
         * @details If the mesh name is invalid, then the @ref type is set to MESH_TYPE_INVALID. Otherwise, the @ref type is set to
         * either MESH_TYPE_COLORMESH or MESH_TYPE_TEXTUREMESH depending on the file extension of the @ref name.
         */
        void SetTypeByName(void){
            type = MESH_TYPE_INVALID;
            if(NameIsValid()){
                std::string lowerCase = StringHelper::ToLowerASCIICopy(name);
                if(StringHelper::EndsWith(lowerCase,".ply")){
                    type = MESH_TYPE_COLORMESH;
                }
                else if(StringHelper::EndsWith(lowerCase,".obj")){
                    type = MESH_TYPE_TEXTUREMESH;
                }
            }
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

    private:
        /**
         * @brief Check if this @ref name is valid.
         * @return True if this @ref name is valid, false otherwise.
         * @details The @ref name is valid if the internal string representing the @ref name contains exactly one dot ('.'),
         * only characters in range 0...9, a...z or A...Z or slashes/underscores ('/','_') before the dot.
         */
        bool NameIsValid(void) const {
            size_t numDots = 0;
            for(auto&& c : name){
                if('.' == c){
                    numDots++;
                    continue;
                }
                else if(('/' == c) && (0 == numDots)){
                    continue;
                }
                else if(('_' == c) && (0 == numDots)){
                    continue;
                }
                else if((c >= '0') && (c <= '9')){
                    continue;
                }
                else if((c >= 'a') && (c <= 'z')){
                    continue;
                }
                else if((c >= 'A') && (c <= 'Z')){
                    continue;
                }
                return false;
            }
            return (1 == numDots);
        }
};

