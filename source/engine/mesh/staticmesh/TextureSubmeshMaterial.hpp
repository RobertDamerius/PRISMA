#pragma once


#include <Common.hpp>
#include <ImageTexture2D.hpp>
#include <ShaderTextureMesh.hpp>


/**
 * @brief Represents a material for a submesh of a texture mesh.
 */
class TextureSubmeshMaterial {
    public:
        std::string name;                 // The name of this material.
        GLfloat shininess;                // Shininess value.
        glm::vec3 diffuseColor;           // Diffuse color.
        glm::vec3 specularColor;          // Specular color.
        glm::vec3 emissionColor;          // Emission color.
        ImageTexture2D diffuseMap;        // 2D texture for the diffuse map.
        ImageTexture2D normalMap;         // 2D texture for the normal map.
        ImageTexture2D emissionMap;       // 2D texture for the emission map.
        ImageTexture2D specularMap;       // 2D texture for the specular map.

        /**
         * @brief Construct a new material for a texture submesh.
         */
        TextureSubmeshMaterial(){
            shininess = 0.0f;
            diffuseColor = glm::vec3(0.0f);
            specularColor = glm::vec3(0.0f);
            emissionColor = glm::vec3(0.0f);
            diffuseMap.SetDefaultDataRGBA({255,255,255,255});
            normalMap.SetDefaultDataRGB({128,128,255});
            emissionMap.SetDefaultDataRGB({255,255,255});
            specularMap.SetDefaultDataRGB({255,255,255});
        }

        /**
         * @brief Bind the diffuse map to GL_TEXTURE0.
         */
        void BindDiffuseMap(void){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE0); );
            diffuseMap.BindTexture();
        }

        /**
         * @brief Apply this material
         * @param[in] shader The texture mesh shader to be used to set material properties.
         * @param[in] diffuseColorMultiplier Optional diffuse color multiplier.
         * @param[in] specularColorMultiplier Specular color multiplier.
         * @param[in] emissionColorMultiplier Emission color multiplier.
         * @param[in] shininessMultiplier Shininess multiplier.
         */
        void Apply(const ShaderTextureMesh& shader, glm::vec3 diffuseColorMultiplier, glm::vec3 specularColorMultiplier, glm::vec3 emissionColorMultiplier, GLfloat shininessMultiplier){
            shader.SetDiffuseColor(diffuseColor * diffuseColorMultiplier);
            shader.SetSpecularColor(specularColor * specularColorMultiplier);
            shader.SetEmissionColor(emissionColor * emissionColorMultiplier);
            shader.SetShininess(shininess * shininessMultiplier);
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE0); );
            diffuseMap.BindTexture();
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE1); );
            normalMap.BindTexture();
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE2); );
            emissionMap.BindTexture();
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE3); );
            specularMap.BindTexture();
        }

        /**
         * @brief Generate GL content and free memory.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            bool success = diffuseMap.Generate(GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true, true, true);
            success &= normalMap.Generate(GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true, true, false);
            success &= emissionMap.Generate(GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true, true, false);
            success &= specularMap.Generate(GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true, true, false);
            if(!success){
                Delete();
            }
            return success;
        }

        /**
         * @brief Delete GL content.
         */
        void Delete(void){
            diffuseMap.Delete();
            normalMap.Delete();
            emissionMap.Delete();
            specularMap.Delete();
        }
};

