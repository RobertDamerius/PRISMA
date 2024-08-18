#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderGUIImage: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct a new GUI image shader.
         */
        ShaderGUIImage(): locationImageTransform(0), locationImageColorScale(0) {}

        /**
         * @brief Generate the shader.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            if(!Shader::Generate(FileName(FILENAME_SHADER_GUIIMAGE))){
                PrintE("Could not generate GUI image shader!\n");
                return false;
            }
            Use();
            locationImageTransform = GetUniformLocation("imageTransform");
            locationImageColorScale = GetUniformLocation("imageColorScale");
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationImageTransform = 0;
        }

        /**
         * @brief Set image transformation value.
         * @param[in] imageTransform The image transformation value.
         */
        void SetImageTransform(glm::vec4 imageTransform) const {
            Uniform4f(locationImageTransform, imageTransform);
        }

        /**
         * @brief Set image color scale value.
         * @param[in] imageColorScale The image color scale value.
         */
        void SetImageColorScale(glm::vec4 imageColorScale) const {
            Uniform4f(locationImageColorScale, imageColorScale);
        }

    protected:
        GLint locationImageTransform;    // Uniform location for imageTransform.
        GLint locationImageColorScale;   // Uniform location for imageColorScale.
};

