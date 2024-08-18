#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>


class ShaderBloomUpsample: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct a new bloom upsample shader.
         */
        ShaderBloomUpsample(){}

        /**
         * @brief Generate the shader.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            if(!Shader::Generate(FileName(FILENAME_SHADER_BLOOMUPSAMPLE))){
                PrintE("Could not generate bloom upsample shader!\n");
                return false;
            }
            Use();
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
        }
};

