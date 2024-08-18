#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderWaterMesh: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct a new water mesh shader.
         */
        ShaderWaterMesh(): locationModelMatrix(0), locationSpecularColor(0), locationShininess(0), locationStrengthDuDvNormal1(0), locationStrengthDuDvNormal2(0), locationReflectionColorMultiplier(0), locationRefractionColorMultiplier(0), locationTextureCoordinateScaling1(0), locationTextureCoordinateScaling2(0), locationTextureCoordinateAnimation1(0), locationTextureCoordinateAnimation2(0), locationDistortionWaterDepthScale(0) {}

        /**
         * @brief Generate the shader.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            std::vector<std::pair<std::string, std::string>> replacement;
            replacement.push_back(std::pair<std::string, std::string>("$UBO_CAMERA$",std::to_string(UBO_CAMERA)));
            if(!Shader::Generate(FileName(FILENAME_SHADER_WATERMESH), replacement)){
                PrintE("Could not generate water mesh shader!\n");
                return false;
            }
            Use();
            locationModelMatrix = GetUniformLocation("modelMatrix");
            locationSpecularColor = GetUniformLocation("specularColor");
            locationShininess = GetUniformLocation("shininess");
            locationStrengthDuDvNormal1 = GetUniformLocation("strengthDuDvNormal1");
            locationStrengthDuDvNormal2 = GetUniformLocation("strengthDuDvNormal2");
            locationReflectionColorMultiplier = GetUniformLocation("reflectionColorMultiplier");
            locationRefractionColorMultiplier = GetUniformLocation("refractionColorMultiplier");
            locationTextureCoordinateScaling1 = GetUniformLocation("textureCoordinateScaling1");
            locationTextureCoordinateScaling2 = GetUniformLocation("textureCoordinateScaling2");
            locationTextureCoordinateAnimation1 = GetUniformLocation("textureCoordinateAnimation1");
            locationTextureCoordinateAnimation2 = GetUniformLocation("textureCoordinateAnimation2");
            locationDistortionWaterDepthScale = GetUniformLocation("distortionWaterDepthScale");
            SetDistortionWaterDepthScale(1.0f);
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationModelMatrix = 0;
            locationSpecularColor = 0;
            locationShininess = 0;
            locationStrengthDuDvNormal1 = 0;
            locationStrengthDuDvNormal2 = 0;
            locationReflectionColorMultiplier = 0;
            locationRefractionColorMultiplier = 0;
            locationTextureCoordinateScaling1 = 0;
            locationTextureCoordinateScaling2 = 0;
            locationTextureCoordinateAnimation1 = 0;
            locationTextureCoordinateAnimation2 = 0;
            locationDistortionWaterDepthScale = 0;
        }

        /**
         * @brief Set model matrix.
         * @param[in] modelMatrix The model matrix.
         */
        void SetModelMatrix(glm::mat4 modelMatrix) const {
            UniformMatrix4fv(locationModelMatrix, GL_FALSE, glm::value_ptr(modelMatrix));
        }

        /**
         * @brief Set the specular color.
         * @param[in] specularColor The specular color to be set.
         */
        void SetSpecularColor(glm::vec3 specularColor) const {
            Uniform3f(locationSpecularColor, specularColor);
        }

        /**
         * @brief Set the shininess value.
         * @param[in] shininess The shininess value to be set.
         */
        void SetShininess(GLfloat shininess){
            Uniform1f(locationShininess, shininess);
        }

        /**
         * @brief Set the strength for the du/dv texture map and normal texture map.
         * @param[in] strengthDuDvNormal1 Strength (1) for the du/dv texture map and normal texture map.
         * @param[in] strengthDuDvNormal2 Strength (2) for the du/dv texture map and normal texture map.
         */
        void SetStrengthDuDvNormal(glm::vec2 strengthDuDvNormal1, glm::vec2 strengthDuDvNormal2){
            Uniform2f(locationStrengthDuDvNormal1, strengthDuDvNormal1);
            Uniform2f(locationStrengthDuDvNormal2, strengthDuDvNormal2);
        }

        /**
         * @brief Set the reflection color multiplier.
         * @param[in] reflectionColorMultiplier The reflection color multiplier to be set.
         */
        void SetReflectionColorMultiplier(glm::vec3 reflectionColorMultiplier){
            Uniform3f(locationReflectionColorMultiplier, reflectionColorMultiplier);
        }

        /**
         * @brief Set the refraction color multiplier.
         * @param[in] refractionColorMultiplier The refraction color multiplier to be set.
         */
        void SetRefractionColorMultiplier(glm::vec3 refractionColorMultiplier){
            Uniform3f(locationRefractionColorMultiplier, refractionColorMultiplier);
        }

        /**
         * @brief Set the texture coordinate scaling.
         * @param[in] textureCoordinateScaling1 The texture coordinate scaling (1) to be set.
         * @param[in] textureCoordinateScaling2 The texture coordinate scaling (2) to be set.
         */
        void SetTextureCoordinateScaling(GLfloat textureCoordinateScaling1, GLfloat textureCoordinateScaling2){
            Uniform1f(locationTextureCoordinateScaling1, textureCoordinateScaling1);
            Uniform1f(locationTextureCoordinateScaling2, textureCoordinateScaling2);
        }

        /**
         * @brief Set the texture coordinate animation.
         * @param[in] textureCoordinateAnimation The texture coordinate animation to be set.
         */
        void SetTextureCoordinateAnimation(glm::vec2 textureCoordinateAnimation1, glm::vec2 textureCoordinateAnimation2){
            Uniform2f(locationTextureCoordinateAnimation1, textureCoordinateAnimation1);
            Uniform2f(locationTextureCoordinateAnimation2, textureCoordinateAnimation2);
        }

        /**
         * @brief Set the distortion water depth scale parameter.
         * @param[in] distortionWaterDepthScale The distortion water depth scale parameter to be set.
         */
        void SetDistortionWaterDepthScale(GLfloat distortionWaterDepthScale){
            Uniform1f(locationDistortionWaterDepthScale, distortionWaterDepthScale);
        }

    protected:
        GLint locationModelMatrix;                   // Uniform location for modelMatrix.
        GLint locationSpecularColor;                 // Uniform location for specularColor.
        GLint locationShininess;                     // Uniform location for shininess.
        GLint locationStrengthDuDvNormal1;           // Uniform location for strengthDuDvNormal1.
        GLint locationStrengthDuDvNormal2;           // Uniform location for strengthDuDvNormal2.
        GLint locationReflectionColorMultiplier;     // Uniform location for reflectionColorMultiplier.
        GLint locationRefractionColorMultiplier;     // Uniform location for refractionColorMultiplier.
        GLint locationTextureCoordinateScaling1;     // Uniform location for textureCoordinateScaling1.
        GLint locationTextureCoordinateScaling2;     // Uniform location for textureCoordinateScaling2.
        GLint locationTextureCoordinateAnimation1;   // Uniform location for textureCoordinateAnimation1.
        GLint locationTextureCoordinateAnimation2;   // Uniform location for textureCoordinateAnimation2.
        GLint locationDistortionWaterDepthScale;     // Uniform location for distortionWaterDepthScale.
};

