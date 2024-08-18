#pragma once


#include <Common.hpp>
#include <ScreenQuad.hpp>
#include <ShaderMinimalEnvironment.hpp>
#include <PrismaState.hpp>
#include <EngineParameter.hpp>
#include <PrismaConfiguration.hpp>


/**
 * @brief Represents the minimal environment renderer.
 */
class MinimalEnvironment {
    public:
        /**
         * @brief Construct a new minimal environment renderer.
         */
        MinimalEnvironment(): fbo(0), cbo(0), width(0), height(0) {}

        /**
         * @brief Generate the minimal environment renderer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @param[in] numShadowCascades Number of shadow cascades in use.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height, GLuint numShadowCascades){
            // if water reflection is disabled, reduce memory to 1-by-1 color buffers
            if(!prismaConfiguration.engine.enableWaterReflection){
                width = height = 1;
            }
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
            // if water reflection is disabled, reduce memory to 1-by-1 color buffers
            if(!prismaConfiguration.engine.enableWaterReflection){
                width = height = 1;
            }
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
            shader.SetAmbientOcclusionEnhance(parameter.ambientOcclusionEnhance);
            shader.SetShadowBiasMinMax(glm::vec2(parameter.shadowBiasMin, parameter.shadowBiasMax));
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
         * @brief Specify the texture ID to be set as emission texture.
         * @param[in] textureID Texture ID of that texture to be used as emission texture.
         */
        void SetEmissionTexture(GLuint textureID){
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
         * @brief Get the scene texture (RGB: scene color, A: depth).
         * @return Texture ID of the scene colorbuffer.
         */
        GLuint GetSceneTexture(void){ return cbo; }

        /**
         * @brief Get the brightness texture size.
         * @return Texture size in pixels.
         */
        glm::ivec2 GetBrightnessTextureSize(void){ return glm::ivec2(width, height); }

    private:
        GLuint fbo;                        // The actual framebuffer object.
        GLuint cbo;                        // Colorbuffer (RGB: scene, A: depth).
        GLsizei width;                     // The width of the framebuffer in pixels.
        GLsizei height;                    // The height of the framebuffer in pixels.
        ScreenQuad screenQuad;             // A screen quad to be used for rendering.
        ShaderMinimalEnvironment shader;   // The environment shader.

        /**
         * @brief Generate colorbuffers and framebuffer and set @ref width and @ref height.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool GenerateFramebuffer(GLsizei width, GLsizei height){
            // if water reflection is disabled, reduce framebuffer to 1-by-1 pixel
            if(!prismaConfiguration.engine.enableWaterReflection){
                width = height = 1;
            }

            // generate the actual frame buffer
            DEBUG_GLCHECK( glGenFramebuffers(1, &fbo); );
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            // colorbuffer: scene color (RGB) and depth (A)
            DEBUG_GLCHECK( glGenTextures(1, &cbo); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cbo); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cbo, 0); );
            // color attachments to use
            const GLenum colorAttachments[1] = {GL_COLOR_ATTACHMENT0};
            DEBUG_GLCHECK( glDrawBuffers(1, &colorAttachments[0]); );
            // check for completeness
            if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)){
                PrintE("Environment framebuffer is not complete!\n");
                Delete();
                return false;
            }
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0); );
            this->width = width;
            this->height = height;

            // if water reflection is disabled, set the 1 pixel color buffer to white such that the relfection/refraction color of the
            // water can be adjusted by the corresponding multipliers
            if(!prismaConfiguration.engine.enableWaterReflection){
                glm::vec3 clearColor = glm::vec3(1.0f);
                DEBUG_GLCHECK( glClearTexImage(cbo, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(clearColor)); );
            }
            return true;
        }

        /**
         * @brief Delete the framebuffer.
         */
        void DeleteFramebuffer(void){
            if(cbo){
                DEBUG_GLCHECK( glDeleteTextures(1, &cbo); );
                cbo = 0;
            }
            if(fbo){
                DEBUG_GLCHECK( glDeleteFramebuffers(1, &fbo); );
                fbo = 0;
            }
            width = 0;
            height = 0;
        }
};

