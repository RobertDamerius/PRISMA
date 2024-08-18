#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderDynamicMesh: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct a new dynamic mesh shader.
         */
        ShaderDynamicMesh(): locationModelMatrix(0), locationDiffuseColorMultiplier(0), locationSpecularColorMultiplier(0), locationEmissionColorMultiplier(0), locationShininessMultiplier(0), locationWaterClipPlane(0) {}

        /**
         * @brief Generate the shader.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            std::vector<std::pair<std::string, std::string>> replacement;
            replacement.push_back(std::pair<std::string, std::string>("$UBO_CAMERA$",std::to_string(UBO_CAMERA)));
            if(!Shader::Generate(FileName(FILENAME_SHADER_DYNAMICMESH), replacement)){
                PrintE("Could not generate dynamic mesh shader!\n");
                return false;
            }
            Use();
            locationModelMatrix = GetUniformLocation("modelMatrix");
            locationDiffuseColorMultiplier = GetUniformLocation("diffuseColorMultiplier");
            locationSpecularColorMultiplier = GetUniformLocation("specularColorMultiplier");
            locationEmissionColorMultiplier = GetUniformLocation("emissionColorMultiplier");
            locationShininessMultiplier = GetUniformLocation("shininessMultiplier");
            locationWaterClipPlane = GetUniformLocation("waterClipPlane");
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationModelMatrix = 0;
            locationDiffuseColorMultiplier = 0;
            locationSpecularColorMultiplier = 0;
            locationEmissionColorMultiplier = 0;
            locationShininessMultiplier = 0;
            locationWaterClipPlane = 0;
        }

        /**
         * @brief Set model matrix.
         * @param[in] modelMatrix The model matrix.
         */
        void SetModelMatrix(glm::mat4 modelMatrix) const {
            UniformMatrix4fv(locationModelMatrix, GL_FALSE, glm::value_ptr(modelMatrix));
        }

        /**
         * @brief Set diffuse color multiplier.
         * @param[in] diffuseColorMultiplier The diffuse color multiplier (RGB).
         */
        void SetDiffuseColorMultiplier(glm::vec3 diffuseColorMultiplier) const {
            Uniform3f(locationDiffuseColorMultiplier, diffuseColorMultiplier);
        }

        /**
         * @brief Set specular color multiplier.
         * @param[in] specularColorMultiplier The specular color multiplier (RGB).
         */
        void SetSpecularColorMultiplier(glm::vec3 specularColorMultiplier) const {
            Uniform3f(locationSpecularColorMultiplier, specularColorMultiplier);
        }

        /**
         * @brief Set emission color multiplier.
         * @param[in] emissionColorMultiplier The emission color multiplier (RGB).
         */
        void SetEmissionColorMultiplier(glm::vec3 emissionColorMultiplier) const {
            Uniform3f(locationEmissionColorMultiplier, emissionColorMultiplier);
        }

        /**
         * @brief Set shininess multiplier.
         * @param[in] shininessMultiplier The shininess multiplier.
         */
        void SetShininessMultiplier(GLfloat shininessMultiplier) const {
            Uniform1f(locationShininessMultiplier, shininessMultiplier);
        }

        /**
         * @brief Set the water clip plane.
         * @param[in] waterClipPlane The water clip plane to be set.
         */
        void SetWaterClipPlane(glm::vec4 waterClipPlane) const {
            Uniform4f(locationWaterClipPlane, waterClipPlane);
        }

    protected:
        GLint locationModelMatrix;               // Uniform location for modelMatrix.
        GLint locationDiffuseColorMultiplier;    // Uniform location for diffuseColorMultiplier.
        GLint locationSpecularColorMultiplier;   // Uniform location for specularColorMultiplier.
        GLint locationEmissionColorMultiplier;   // Uniform location for emissionColorMultiplier.
        GLint locationShininessMultiplier;       // Uniform location for shininessMultiplier.
        GLint locationWaterClipPlane;            // Uniform location for waterClipPlane.
};

