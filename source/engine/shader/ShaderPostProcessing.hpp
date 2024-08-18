#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderPostProcessing: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Create post processing shader.
         */
        ShaderPostProcessing(): locationInvGamma(0), locationBloomStrength(0), locationDitheringNoiseGranularity(0), locationExposure(0), locationToneMappingStrength(0){}

        /**
         * @brief Generate the shader.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            if(!Shader::Generate(FileName(FILENAME_SHADER_POSTPROCESSING))){
                PrintE("Could not generate post processing shader!\n");
                return false;
            }
            Use();
            locationInvGamma = GetUniformLocation("invGamma");
            locationBloomStrength = GetUniformLocation("bloomStrength");
            locationDitheringNoiseGranularity = GetUniformLocation("ditheringNoiseGranularity");
            locationExposure = GetUniformLocation("exposure");
            locationToneMappingStrength = GetUniformLocation("toneMappingStrength");
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationInvGamma = 0;
            locationBloomStrength = 0;
            locationDitheringNoiseGranularity = 0;
            locationExposure = 0;
            locationToneMappingStrength = 0;
        }

        /**
         * @brief Set gamma value.
         * @param[in] gamma The gamma value to be set.
         */
        void SetGamma(GLfloat gamma) const {
            Uniform1f(locationInvGamma, 1.0f / gamma);
        }

        /**
         * @brief Set bloom strength.
         * @param[in] bloomStrength The bloom strength to be set.
         */
        void SetBloomStrength(GLfloat bloomStrength) const {
            Uniform1f(locationBloomStrength, bloomStrength);
        }

        /**
         * @brief Set the dithering noise granularity.
         * @param[in] ditheringNoiseGranularity The dithering noise granularity to be set.
         */
        void SetDitheringNoiseGranularity(GLfloat ditheringNoiseGranularity) const {
            Uniform1f(locationDitheringNoiseGranularity, ditheringNoiseGranularity);
        }

        /**
         * @brief Set exposure.
         * @param[in] exposure The exposure to be set.
         */
        void SetExposure(GLfloat exposure) const {
            Uniform1f(locationExposure, exposure);
        }

        /**
         * @brief Set tone mapping strength.
         * @param[in] toneMappingStrength The tone mapping strength to be set.
         */
        void SetToneMappingStrength(GLfloat toneMappingStrength) const {
            Uniform1f(locationToneMappingStrength, toneMappingStrength);
        }

    private:
        GLint locationInvGamma;                    // Uniform location for invGamma.
        GLint locationBloomStrength;               // Uniform location for bloomStrength.
        GLint locationDitheringNoiseGranularity;   // Uniform location for ditheringNoiseGranularity.
        GLint locationExposure;                    // Uniform location for exposure.
        GLint locationToneMappingStrength;         // Uniform location for toneMappingStrength.
};

