#pragma once


#include <Common.hpp>
#include <ShaderBloomDownsample.hpp>
#include <ShaderBloomUpsample.hpp>
#include <ScreenQuad.hpp>
#include <PrismaConfiguration.hpp>


/**
 * @brief The renderer for a bloom effect.
 */
class Bloom {
    public:
        /**
         * @brief Construct a new bloom effect renderer.
         */
        Bloom(){
            fbo = 0;
        }

        /**
         * @brief Generate the rendering content, such as framebuffer object and shader, for the bloom effect renderer.
         * @param[in] width Framebuffer width in pixels.
         * @param[in] height Framebuffer height in pixels.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height){
            // if bloom is disabled, reduce memory to 1-by-1 color buffer
            if(!prismaConfiguration.engine.enableBloom){
                width = height = 1;
            }
            bool success = true;
            screenQuad.Generate();
            success &= shaderDownsample.Generate();
            success &= shaderUpsample.Generate();
            success &= GenerateMipChain(width, height);
            if(!success){
                Delete();
            }
            return success;
        }

        /**
         * @brief Delete the rendering content bloom effect renderer.
         */
        void Delete(void){
            screenQuad.Delete();
            shaderDownsample.Delete();
            shaderUpsample.Delete();
            DeleteMipChain();
        }

        /**
         * @brief Resize the internal framebuffers of the bloom effect renderer.
         * @param[in] width Framebuffer width in pixels.
         * @param[in] height Framebuffer height in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            // if bloom is disabled, reduce memory to 1-by-1 color buffer
            if(!prismaConfiguration.engine.enableBloom){
                width = height = 1;
            }
            DeleteMipChain();
            (void) GenerateMipChain(width, height);
        }

        /**
         * @brief Use the bloom renderer by binding the framebuffer.
         */
        void Use(void){
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            DEBUG_GLCHECK( glCullFace(GL_BACK); );
            DEBUG_GLCHECK( glDisable(GL_STENCIL_TEST); );
            DEBUG_GLCHECK( glDisable(GL_DEPTH_TEST); );
            DEBUG_GLCHECK( glDisable(GL_BLEND); );
            DEBUG_GLCHECK( glClearColor(0.0f, 0.0f, 0.0f, 0.0f); );
            DEBUG_GLCHECK( glClear(GL_COLOR_BUFFER_BIT); );
        }

        /**
         * @brief Specify the input texture and draw the bloom effect.
         * @param[in] inputTexture Texture ID of that texture to be used as input texture for the bloom effect.
         * @param[in] inputTextureSize Texture size if the input texture in pixels (width, height).
         */
        void Draw(const GLuint inputTexture, const glm::ivec2 inputTextureSize){
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE0); );
            Downsampling(inputTexture, inputTextureSize);
            DEBUG_GLCHECK( glEnable(GL_BLEND); );
            DEBUG_GLCHECK( glBlendFunc(GL_ONE, GL_ONE); );
            DEBUG_GLCHECK( glBlendEquation(GL_FUNC_ADD); );
            Upsampling();
        }

        /**
         * @brief Get the bloom texture.
         * @return Texture ID of the bloom colorbuffer.
         */
        GLuint GetBloomTexture(void) const {
            GLuint result = 0;
            if(!mipChain.empty()){
                result = mipChain[0].textureID;
            }
            return result;
        }

    private:
        struct BloomMip {
            GLuint textureID;                     // GL texture ID for the mip.
            glm::vec2 textureSize;                // Texture size for the mip (width, height).
        };
        std::vector<BloomMip> mipChain;           // Chain of all mips.
        ScreenQuad screenQuad;                    // A screen quad to be used for rendering.
        GLuint fbo;                               // Internal framebuffer object.
        ShaderBloomDownsample shaderDownsample;   // The shader for downsampling.
        ShaderBloomUpsample shaderUpsample;       // The shader for upsampling.

        /**
         * @brief Generate the mip chain of the bloom effect.
         * @param[in] width Framebuffer width in pixels.
         * @param[in] height Framebuffer height in pixels.
         * @return True if success, false otherwise.
         */
        bool GenerateMipChain(GLsizei width, GLsizei height){
            glm::vec2 mipSize(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
            DEBUG_GLCHECK( glGenFramebuffers(1, &fbo); );
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            GLuint maxNumMips = prismaConfiguration.engine.maxNumBloomMips;
            if(!prismaConfiguration.engine.enableBloom){
                maxNumMips = 0;
            }
            for(GLuint i = 0; i < maxNumMips; ++i){
                BloomMip mip;
                mipSize /= 2.0f;
                mip.textureSize = mipSize;
                GLsizei textureWidth = static_cast<GLsizei>(mip.textureSize.x);
                GLsizei textureHeight = static_cast<GLsizei>(mip.textureSize.y);
                if((textureWidth < 1) || (textureHeight < 1)){
                    break;
                }
                DEBUG_GLCHECK( glGenTextures(1, &mip.textureID); );
                DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, mip.textureID); );
                DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, nullptr); );
                DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); );
                DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); );
                DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
                DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
                mipChain.push_back(mip);
            }
            if(!mipChain.empty()){
                DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mipChain[0].textureID, 0); );
                const GLenum colorAttachments[1] = {GL_COLOR_ATTACHMENT0};
                DEBUG_GLCHECK( glDrawBuffers(1, &colorAttachments[0]); );
                if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)){
                    PrintE("Framebuffer for bloom is not complete!\n");
                    DeleteMipChain();
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Delete the mip chain that has been generated via @ref GenerateMipChain.
         */
        void DeleteMipChain(void){
            for(auto&& mip : mipChain){
                DEBUG_GLCHECK( glDeleteTextures(1, &mip.textureID); );
            }
            mipChain.clear();
            if(fbo){
                DEBUG_GLCHECK( glDeleteFramebuffers(1, &fbo); );
                fbo = 0;
            }
        }

        /**
         * @brief Perform all downsampling steps.
         * @param[in] inputTexture Texture ID that indicates the input texture for the bloom effect.
         * @param[in] inputTextureSize Texture size if the input texture in pixels (width, height).
         */
        void Downsampling(const GLuint inputTexture, const glm::ivec2 inputTextureSize){
            glm::vec2 textureSize = inputTextureSize;
            GLuint textureID = inputTexture;
            shaderDownsample.Use();
            for(GLint i = 0; i < static_cast<GLint>(mipChain.size()); ++i){
                const BloomMip& mip = mipChain[i];
                shaderDownsample.SetMipLevel(i);
                shaderDownsample.SetMipSize(textureSize);
                DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
                DEBUG_GLCHECK( glViewport(0, 0, static_cast<GLsizei>(mip.textureSize.x), static_cast<GLsizei>(mip.textureSize.y)); );
                DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.textureID, 0); );
                screenQuad.Draw();
                textureSize = mip.textureSize;
                textureID = mip.textureID;
            }
        }

        /**
         * @brief Perform all upsampling steps.
         */
        void Upsampling(void){
            shaderUpsample.Use();
            for(GLint i = static_cast<GLint>(mipChain.size()) - 1; i > 0; --i){
                const BloomMip& thisMip = mipChain[i];
                const BloomMip& nextMip = mipChain[i - 1];
                DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, thisMip.textureID); );
                DEBUG_GLCHECK( glViewport(0, 0, static_cast<GLsizei>(nextMip.textureSize.x), static_cast<GLsizei>(nextMip.textureSize.y)); );
                DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.textureID, 0); );
                screenQuad.Draw();
            }
        }
};

