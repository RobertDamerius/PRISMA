#pragma once


#include <Common.hpp>
#include <ScreenQuad.hpp>
#include <ShaderSSAO.hpp>
#include <EngineParameter.hpp>
#include <PrismaConfiguration.hpp>


/**
 * @brief Represents the ambient occlusion renderer.
 * @details Screen-space ambient occlusion (SSAO) is used.
 */
class AmbientOcclusion {
    public:
        /**
         * @brief Construct a new ambient occlusion renderer.
         */
        AmbientOcclusion(): width(0), height(0), fbo(0), cbo(0), noiseTexture(0) {}

        /**
         * @brief Generate the ambient occlusion renderer by generating framebuffer, shader, etc.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @param[in] kernelSize The kernel size for the SSAO shader. The default value is 16.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height, GLuint kernelSize = 16){
            // if ambient occlusion is disabled, reduce memory to 1-by-1 texture and set kernel size to 1
            if(!prismaConfiguration.engine.enableAmbientOcclusion){
                width = height = 1;
                kernelSize = 1;
            }

            // actual generation of framebuffer and samples
            bool success = true;
            screenQuad.Generate();
            success &= shaderSSAO.Generate(width, height, kernelSize);
            success &= GenerateFramebuffer(width, height);
            GenerateRandomSamples(kernelSize);
            GenerateNoiseTexture();
            if(!success){
                Delete();
            }
            return success;
        }

        /**
         * @brief Delete the ambient occlusion renderer and its framebuffer, shader, etc.
         */
        void Delete(void){
            screenQuad.Delete();
            shaderSSAO.Delete();
            DeleteFramebuffer();
            DeleteNoiseTexture();
        }

        /**
         * @brief Resize the framebuffer of the ambient occlusion renderer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            // if ambient occlusion is disabled, reduce memory to 1-by-1 texture
            if(!prismaConfiguration.engine.enableAmbientOcclusion){
                width = height = 1;
            }
            DeleteFramebuffer();
            (void) GenerateFramebuffer(width, height);
            shaderSSAO.Use();
            shaderSSAO.SetResolution(width, height);
        }

        /**
         * @brief Use the ambient occlusion renderer and bind the framebuffer.
         */
        void Use(void){
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE2); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, noiseTexture); );
            DEBUG_GLCHECK( glViewport(0, 0, width, height); );
            DEBUG_GLCHECK( glCullFace(GL_BACK); );
            DEBUG_GLCHECK( glDisable(GL_STENCIL_TEST); );
            DEBUG_GLCHECK( glDisable(GL_DEPTH_TEST); );
            DEBUG_GLCHECK( glDisable(GL_BLEND); );
            DEBUG_GLCHECK( glClearColor(1.0f, 1.0f, 1.0f, 1.0f); );
            DEBUG_GLCHECK( glClear(GL_COLOR_BUFFER_BIT); );
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
         * @brief Use the shader and draw the ambient occlusion.
         * @param[in] parameter Runtime parameter of the PRISMA engine.
         */
        void Draw(const EngineParameter& parameter){
            shaderSSAO.Use();
            shaderSSAO.SetOcclusionRadius(parameter.ambientOcclusionRadius);
            shaderSSAO.SetOcclusionBias(parameter.ambientOcclusionBias);
            screenQuad.Draw();
        }

        /**
         * @brief Get the occlusion texture.
         * @return Texture ID of the occlusion colorbuffer.
         */
        GLuint GetOcclusionTexture(void) const { return cbo; }

    private:
        GLsizei width;           // Width of the framebuffer in pixels.
        GLsizei height;          // Height of the framebuffer in pixels.
        GLuint fbo;              // Internal framebuffer object.
        GLuint cbo;              // Internal colorbuffer object.
        GLuint noiseTexture;     // Texture ID of the 4-by-4 noise texture that contains random rotation vectors.
        ScreenQuad screenQuad;   // Screen quad to be used for rendering.
        ShaderSSAO shaderSSAO;   // Shader to be used for screen-space ambient occlusion.

        /**
         * @brief Generate random samples for the SSAO kernel and upload them to the shader program.
         * @param[in] kernelSize The kernel size for the SSAO shader.
         */
        void GenerateRandomSamples(GLuint kernelSize){
            std::vector<glm::vec3> ssaoKernel;
            std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
            std::default_random_engine generator;
            for(GLuint i = 0; i < kernelSize; ++i){
                glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
                GLfloat scale = static_cast<GLfloat>(static_cast<double>(i) / static_cast<double>(kernelSize));
                sample = (0.1f + 0.9f * scale * scale) * glm::normalize(sample);
                sample *= randomFloats(generator);
                ssaoKernel.push_back(sample);
            }
            shaderSSAO.Use();
            shaderSSAO.SetSamples(ssaoKernel);
        }

        /**
         * @brief Generate the framebuffer for the ambient occlusion renderer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool GenerateFramebuffer(GLsizei width, GLsizei height){
            DEBUG_GLCHECK( glGenFramebuffers(1, &fbo); );
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            DEBUG_GLCHECK( glGenTextures(1, &cbo); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cbo); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cbo, 0); );
            // check for completeness
            if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)){
                PrintE("SSAO framebuffer is not complete!\n");
                DeleteFramebuffer();
                return false;
            }
            this->width = width;
            this->height = height;
            return true;
        }

        /**
         * @brief Delete the framebuffer.
         */
        void DeleteFramebuffer(void){
            if(fbo){
                DEBUG_GLCHECK( glDeleteFramebuffers(1, &fbo); );
                fbo = 0;
            }
            width = 0;
            height = 0;
        }

        /**
         * @brief Generate the noise texture.
         */
        void GenerateNoiseTexture(void){
            std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
            std::default_random_engine generator;
            std::vector<glm::vec3> ssaoNoise;
            for(int i = 0; i < 16; ++i){
                glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f); // random rotation vectors around z axis
                ssaoNoise.push_back(noise);
            }
            DEBUG_GLCHECK( glGenTextures(1, &noiseTexture); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, noiseTexture); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); );
        }

        /**
         * @brief Delete the noise texture.
         */
        void DeleteNoiseTexture(void){
            if(noiseTexture){
                DEBUG_GLCHECK( glDeleteTextures(1, &noiseTexture); );
                noiseTexture = 0;
            }
        }
};

