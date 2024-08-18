#pragma once


#include <Common.hpp>
#include <ScreenQuad.hpp>
#include <ShaderEnvironment.hpp>
#include <PrismaState.hpp>
#include <EngineParameter.hpp>
#include <PrismaConfiguration.hpp>


/**
 * @brief Represents the environment renderer.
 */
class Environment {
    public:
        /**
         * @brief Construct a new environment renderer.
         */
        Environment(): fbo(0), cboScene(0), cboBrightness(0), width(0), height(0) {}

        /**
         * @brief Generate the environment renderer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @param[in] numShadowCascades Number of shadow cascades in use.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height, GLuint numShadowCascades){
            bool success = true;
            screenQuad.Generate();
            success &= shader.Generate(prismaConfiguration.engine.maxNumDirectionalLights, prismaConfiguration.engine.maxNumPointLights, prismaConfiguration.engine.maxNumSpotLights, numShadowCascades);
            success &= GenerateFramebuffer(width, height);
            if(!success){
                Delete();
            }
            return success;
        }

        /**
         * @brief Delete the environment renderer.
         */
        void Delete(void){
            screenQuad.Delete();
            shader.Delete();
            DeleteFramebuffer();
        }

        /**
         * @brief Resize the G-buffer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            DeleteFramebuffer();
            (void) GenerateFramebuffer(width, height);
        }

        /**
         * @brief Use the environment renderer by binding the framebuffer and using the shader.
         * @param[in] parameter Runtime parameter of the PRISMA engine.
         */
        void Use(const EngineParameter& parameter){
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            DEBUG_GLCHECK( glViewport(0, 0, width, height); );
            DEBUG_GLCHECK( glCullFace(GL_BACK); );
            DEBUG_GLCHECK( glDisable(GL_STENCIL_TEST); );
            DEBUG_GLCHECK( glDisable(GL_DEPTH_TEST); );
            DEBUG_GLCHECK( glDisable(GL_BLEND); );
            DEBUG_GLCHECK( glClearColor(0.0f, 0.0f, 0.0f, 0.0f); );
            DEBUG_GLCHECK( glClear(GL_COLOR_BUFFER_BIT); );
            shader.Use();
            shader.SetBrightnessThreshold(parameter.bloomBrightnessThreshold);
            shader.SetAmbientOcclusionEnhance(parameter.ambientOcclusionEnhance);
            shader.SetShadowBiasMinMax(glm::vec2(parameter.shadowBiasMin, parameter.shadowBiasMax));
            shader.SetRemoveShadowOnWater(parameter.removeShadowOnWater);
        }

        /**
         * @brief Apply the scene to the environment, set visible lights.
         * @param[in] prismaState The PRISMA state that contains the scene to be applied to the environment.
         */
        void ApplyScene(PrismaState& prismaState){
            prismaState.atmosphere.Apply(shader, prismaState.camera);
            prismaState.lightLibrary.ApplyVisibleLights(shader, prismaState.camera);
        }

        /**
         * @brief Set the far planes of shadow cascades to the environment shader.
         * @param[in] farPlanes Far planes to set as uniforms in the environment shader.
         */
        void SetShadowCascadeFarPlanes(const std::vector<GLfloat>& farPlanes){
            shader.SetShadowCascadeFarPlanes(farPlanes);
        }

        /**
         * @brief Render the environment.
         */
        void Draw(void){
            screenQuad.Draw();
        }

        /**
         * @brief Specify the texture ID to be set as position texture.
         * @param[in] textureID Texture ID of that texture to be used as position texture.
         */
        void SetPositionTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE0); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as normal texture.
         * @param[in] textureID Texture ID of that texture to be used as normal texture.
         */
        void SetNormalTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE1); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as diffuse texture.
         * @param[in] textureID Texture ID of that texture to be used as diffuse texture.
         */
        void SetDiffuseTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE2); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as emission/reflection indicator texture.
         * @param[in] textureID Texture ID of that texture to be used as emission/reflection indicator texture.
         */
        void SetEmissionReflectionTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE3); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as specular/shininess texture.
         * @param[in] textureID Texture ID of that texture to be used as specular/shininess texture.
         */
        void SetSpecularShininessTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE4); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as face normal texture.
         * @param[in] textureID Texture ID of that texture to be used as face normal texture.
         */
        void SetFaceNormalTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE5); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as occlusion texture.
         * @param[in] textureID Texture ID of that texture to be used as occlusion texture.
         */
        void SetOcclusionTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE6); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
        }

        /**
         * @brief Specify the texture ID to be set as shadow map texture.
         * @param[in] textureID Texture ID of that texture to be used as shadow map texture.
         */
        void SetShadowMapTexture(GLuint textureID){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE7); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D_ARRAY, textureID); );
        }

        /**
         * @brief Get the scene texture.
         * @return Texture ID of the scene colorbuffer.
         */
        GLuint GetSceneTexture(void){ return cboScene; }

        /**
         * @brief Get the brightness texture.
         * @return Texture ID of the brightness colorbuffer.
         */
        GLuint GetBrightnessTexture(void){ return cboBrightness; }

        /**
         * @brief Get the brightness texture size.
         * @return Texture size in pixels.
         */
        glm::ivec2 GetBrightnessTextureSize(void){ return glm::ivec2(width, height); }

        /**
         * @brief Get the environment shader.
         * @return The environment shader.
         */
        const ShaderEnvironment& GetShader(void){ return std::cref(shader); }

    private:
        GLuint fbo;                 // The actual framebuffer object.
        GLuint cboScene;            // Colorbuffer (RGB scene albedo color).
        GLuint cboBrightness;       // Colorbuffer (RGB scene brightness color).
        GLsizei width;              // The width of the framebuffer in pixels.
        GLsizei height;             // The height of the framebuffer in pixels.
        ScreenQuad screenQuad;      // A screen quad to be used for rendering.
        ShaderEnvironment shader;   // The environment shader.

        /**
         * @brief Generate colorbuffers and framebuffer and set @ref width and @ref height.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool GenerateFramebuffer(GLsizei width, GLsizei height){
            DEBUG_GLCHECK( glGenFramebuffers(1, &fbo); );
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            // colorbuffer 1: scene color (RGB)
            DEBUG_GLCHECK( glGenTextures(1, &cboScene); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cboScene); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cboScene, 0); );
            // colorbuffer 2: scene brightness (RGB)
            DEBUG_GLCHECK( glGenTextures(1, &cboBrightness); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cboBrightness); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, cboBrightness, 0); );
            // color attachments to use
            const GLenum colorAttachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
            DEBUG_GLCHECK( glDrawBuffers(2, &colorAttachments[0]); );
            // check for completeness
            if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)){
                PrintE("Environment framebuffer is not complete!\n");
                Delete();
                return false;
            }
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0); );
            this->width = width;
            this->height = height;
            return true;
        }

        /**
         * @brief Delete the framebuffer.
         */
        void DeleteFramebuffer(void){
            if(cboScene){
                DEBUG_GLCHECK( glDeleteTextures(1, &cboScene); );
                cboScene = 0;
            }
            if(cboBrightness){
                DEBUG_GLCHECK( glDeleteTextures(1, &cboBrightness); );
                cboBrightness = 0;
            }
            if(fbo){
                DEBUG_GLCHECK( glDeleteFramebuffers(1, &fbo); );
                fbo = 0;
            }
            width = 0;
            height = 0;
        }
};

