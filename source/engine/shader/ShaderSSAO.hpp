#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderSSAO: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct a new ambient occlusion shader.
         */
        ShaderSSAO(): locationKernelSize(0), locationNoiseScale(0), locationOcclusionRadius(0), locationOcclusionBias(0){}

        /**
         * @brief Generate the shader.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @param[in] kernelSize The kernel size for the SSAO shader.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height, GLuint kernelSize){
            constexpr double noiseTextureDimension = 4.0;
            std::vector<std::pair<std::string, std::string>> replacement;
            replacement.push_back(std::pair<std::string, std::string>("$UBO_CAMERA$",std::to_string(UBO_CAMERA)));
            replacement.push_back(std::pair<std::string, std::string>("$SSAO_KERNEL_SIZE$",std::to_string(kernelSize)));
            if(!Shader::Generate(FileName(FILENAME_SHADER_SSAO), replacement)){
                PrintE("Could not generate SSAO shader!\n");
                return false;
            }
            Use();
            locationSamples.resize(kernelSize);
            for(GLuint i = 0; i < kernelSize; ++i){
                locationSamples[i] = GetUniformLocation("samples[" + std::to_string(i) + "]");
            }
            locationKernelSize = GetUniformLocation("kernelSize");
            locationNoiseScale = GetUniformLocation("noiseScale");
            locationOcclusionRadius = GetUniformLocation("occlusionRadius");
            locationOcclusionBias = GetUniformLocation("occlusionBias");
            Uniform1ui(locationKernelSize, kernelSize);
            Uniform2f(locationNoiseScale, glm::vec2(width / noiseTextureDimension, height / noiseTextureDimension));
            Uniform1f(locationOcclusionRadius, 1.0f);
            Uniform1f(locationOcclusionBias, 0.025f);
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationSamples.clear();
        }

        /**
         * @brief Set the kernel samples.
         * @param[in] samples Random sample kernels.
         */
        void SetSamples(const std::vector<glm::vec3>& samples) const {
            GLuint kernelSize = static_cast<GLuint>(std::min(locationSamples.size(), samples.size()));
            for(GLuint i = 0; i < kernelSize; ++i){
                Uniform3f(locationSamples[i], samples[i]);
            }
            Uniform1ui(locationKernelSize, kernelSize);
        }

        /**
         * @brief Set the current framebuffer resolution to adjust the noise scale.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         */
        void SetResolution(GLsizei width, GLsizei height){
            constexpr double noiseTextureDimension = 4.0;
            Uniform2f(locationNoiseScale, glm::vec2(width / noiseTextureDimension, height / noiseTextureDimension));
        }

        /**
         * @brief Set the occlusion radius.
         * @param[in] occlusionRadius The occlusion radius to be set.
         */
        void SetOcclusionRadius(GLfloat occlusionRadius){
            Uniform1f(locationOcclusionRadius, occlusionRadius);
        }

        /**
         * @brief Set the occlusion bias.
         * @param[in] occlusionBias The occlusion bias.
         */
        void SetOcclusionBias(GLfloat occlusionBias){
            Uniform1f(locationOcclusionBias, occlusionBias);
        }

    protected:
        GLint locationKernelSize;             // Location for kernelSize.
        GLint locationNoiseScale;             // Location for noiseScale.
        GLint locationOcclusionRadius;        // Location for occlusionRadius;
        GLint locationOcclusionBias;          // Location for occlusionBias;
        std::vector<GLint> locationSamples;   // Location for samples.
};

