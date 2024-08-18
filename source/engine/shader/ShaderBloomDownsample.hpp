#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderBloomDownsample: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct a new bloom downsample shader.
         */
        ShaderBloomDownsample(): locationTexelSize(0), locationMipLevel(0) {}

        /**
         * @brief Generate the shader.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            if(!Shader::Generate(FileName(FILENAME_SHADER_BLOOMDOWNSAMPLE))){
                PrintE("Could not generate bloom downsample shader!\n");
                return false;
            }
            Use();
            locationTexelSize = GetUniformLocation("texelSize");
            locationMipLevel = GetUniformLocation("mipLevel");
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationTexelSize = 0;
            locationMipLevel = 0;
        }

        /**
         * @brief Set the size of the mip input texture.
         * @param[in] mipSize Size of the mip input texture.
         */
        void SetMipSize(glm::vec2 mipSize) const {
            glm::vec2 texelSize = 1.0f / mipSize;
            Uniform2f(locationTexelSize, texelSize);
        }

        /**
         * @brief Set mip level.
         * @param[in] mipLevel The mip level.
         */
        void SetMipLevel(GLint mipLevel) const {
            Uniform1i(locationMipLevel, mipLevel);
        }

    protected:
        GLint locationTexelSize;   // Uniform location for texelSize.
        GLint locationMipLevel;    // Uniform location for nipLevel.
};

