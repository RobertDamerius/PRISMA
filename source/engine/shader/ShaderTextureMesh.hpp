#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderTextureMesh: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct a new texture mesh shader.
         */
        ShaderTextureMesh(): locationModelMatrix(0), locationDiffuseColor(0), locationSpecularColor(0), locationEmissionColor(0), locationShininess(0), locationWaterClipPlane(0){}

        /**
         * @brief Generate the shader.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            std::vector<std::pair<std::string, std::string>> replacement;
            replacement.push_back(std::pair<std::string, std::string>("$UBO_CAMERA$",std::to_string(UBO_CAMERA)));
            if(!Shader::Generate(FileName(FILENAME_SHADER_TEXTUREMESH), replacement)){
                PrintE("Could not generate texture mesh shader!\n");
                return false;
            }
            Use();
            locationModelMatrix = GetUniformLocation("modelMatrix");
            locationDiffuseColor = GetUniformLocation("diffuseColor");
            locationSpecularColor = GetUniformLocation("specularColor");
            locationEmissionColor = GetUniformLocation("emissionColor");
            locationShininess = GetUniformLocation("shininess");
            locationWaterClipPlane = GetUniformLocation("waterClipPlane");
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationModelMatrix = 0;
            locationDiffuseColor = 0;
            locationSpecularColor = 0;
            locationEmissionColor = 0;
            locationShininess = 0;
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
         * @brief Set the diffuse color.
         * @param[in] diffuseColor The diffuse color (RGB).
         */
        void SetDiffuseColor(glm::vec3 diffuseColor) const {
            Uniform3f(locationDiffuseColor, diffuseColor);
        }

        /**
         * @brief Set the specular color.
         * @param[in] specularColor The specular color (RGB).
         */
        void SetSpecularColor(glm::vec3 specularColor) const {
            Uniform3f(locationSpecularColor, specularColor);
        }

        /**
         * @brief Set the emission color.
         * @param[in] emissionColor The emission color (RGB).
         */
        void SetEmissionColor(glm::vec3 emissionColor) const {
            Uniform3f(locationEmissionColor, emissionColor);
        }

        /**
         * @brief Set the shininess value.
         * @param[in] shininess The shininess to be set.
         */
        void SetShininess(GLfloat shininess) const {
            Uniform1f(locationShininess, shininess);
        }

        /**
         * @brief Set the water clip plane.
         * @param[in] waterClipPlane The water clip plane to be set.
         */
        void SetWaterClipPlane(glm::vec4 waterClipPlane) const {
            Uniform4f(locationWaterClipPlane, waterClipPlane);
        }

    protected:
        GLint locationModelMatrix;       // Uniform location for modelMatrix.
        GLint locationDiffuseColor;      // Uniform location for diffuseColor.
        GLint locationSpecularColor;     // Uniform location for specularColor.
        GLint locationEmissionColor;     // Uniform location for emissionColor.
        GLint locationShininess;         // Uniform location for shininess.
        GLint locationWaterClipPlane;    // Uniform location for waterClipPlane.
};

