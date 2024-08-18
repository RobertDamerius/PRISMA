#pragma once


#include <Common.hpp>
#include <ScreenQuad.hpp>
#include <ShaderPostProcessing.hpp>
#include <EngineParameter.hpp>


/**
 * @brief Represents the post processing renderer.
 */
class PostProcessing {
    public:
        /**
         * @brief Construct a new post processing renderer.
         */
        PostProcessing(): width(0), height(0) {}

        /**
         * @brief Generate the post processing renderer by generating shader and screen squad.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height){
            this->width = width;
            this->height = height;
            screenQuad.Generate();
            return shader.Generate();
        }

        /**
         * @brief Delete the post processing renderer.
         */
        void Delete(void){
            screenQuad.Delete();
            shader.Delete();
        }

        /**
         * @brief Resize the post processing renderer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            this->width = width;
            this->height = height;
        }

        /**
         * @brief Use the post processing renderer.
         */
        void Use(void){
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0); );
            DEBUG_GLCHECK( glViewport(0, 0, width, height); );
            DEBUG_GLCHECK( glCullFace(GL_BACK); );
            DEBUG_GLCHECK( glDisable(GL_STENCIL_TEST); );
            DEBUG_GLCHECK( glDisable(GL_DEPTH_TEST); );
            DEBUG_GLCHECK( glDisable(GL_BLEND); );
            DEBUG_GLCHECK( glClearColor(0.0f, 0.0f, 0.0f, 0.0f); );
            DEBUG_GLCHECK( glClear(GL_COLOR_BUFFER_BIT); );
        }

        /**
         * @brief Draw the post processing effect.
         * @param[in] parameter Runtime parameter of the PRISMA engine.
         */
        void Draw(const EngineParameter& parameter){
            shader.Use();
            shader.SetBloomStrength(parameter.bloomStrength);
            shader.SetGamma(parameter.gamma);
            shader.SetDitheringNoiseGranularity(parameter.ditheringNoiseGranularity);
            shader.SetExposure(parameter.exposure);
            shader.SetToneMappingStrength(parameter.toneMappingStrength);
            screenQuad.Draw();
        }

        /**
         * @brief Specify the texture ID to be set as GUI texture.
         * @param[in] textureID Texture ID of that texture to be used as GUI texture.
         */
        void SetGUITexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE0); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as scene texture.
         * @param[in] textureID Texture ID of that texture to be used as scene texture.
         */
        void SetSceneTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE1); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as bloom texture.
         * @param[in] textureID Texture ID of that texture to be used as bloom texture.
         */
        void SetBloomTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE2); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

    private:
        GLsizei width;                 // The width of the framebuffer in pixels.
        GLsizei height;                // The height of the framebuffer in pixels.
        ScreenQuad screenQuad;         // The screen quad to be used for rendering.
        ShaderPostProcessing shader;   // The post processing shader.
};

