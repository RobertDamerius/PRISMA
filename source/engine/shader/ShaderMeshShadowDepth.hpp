#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderMeshShadowDepth: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct a new mesh shadow depth shader.
         */
        ShaderMeshShadowDepth(): locationModelMatrix(0){}

        /**
         * @brief Generate the shader.
         * @param[in] numShadowCascades The number of cascaded shadow maps in use.
         * @return True if success, false otherwise.
         */
        bool Generate(GLuint numShadowCascades){
            std::vector<std::pair<std::string, std::string>> replacement;
            replacement.push_back(std::pair<std::string, std::string>("$UBO_CAMERA$",std::to_string(UBO_CAMERA)));
            replacement.push_back(std::pair<std::string, std::string>("$UBO_LIGHTMATRICES$",std::to_string(UBO_LIGHTMATRICES)));
            replacement.push_back(std::pair<std::string, std::string>("$NUMBER_OF_SHADOW_CASCADES$",std::to_string(numShadowCascades)));
            if(!Shader::Generate(FileName(FILENAME_SHADER_MESHSHADOWDEPTH), replacement)){
                PrintE("Could not generate color mesh shadow shader!\n");
                return false;
            }
            Use();
            locationModelMatrix = GetUniformLocation("modelMatrix");
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationModelMatrix = 0;
        }

        /**
         * @brief Set model matrix.
         * @param[in] modelMatrix The model matrix.
         */
        void SetModelMatrix(glm::mat4 modelMatrix) const {
            UniformMatrix4fv(locationModelMatrix, GL_FALSE, glm::value_ptr(modelMatrix));
        }

    protected:
        GLint locationModelMatrix;   // Uniform location for modelMatrix.
};

