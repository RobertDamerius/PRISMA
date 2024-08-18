#pragma once


#include <Common.hpp>
#include <GUIDisplay.hpp>


class GUIRenderer {
    public:
        /**
         * @brief Construct a new GUI renderer.
         */
        GUIRenderer(): width(0), height(0), cbo(0), rbo(0), fbo(0){}

        /**
         * @brief Generate the GUI renderer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height){
            return GenerateFramebuffer(width, height) && guiDisplay.Generate(width, height);
        }

        /**
         * @brief Delete the GUI renderer.
         */
        void Delete(void){
            guiDisplay.Delete();
            DeleteFramebuffer();
        }

        /**
         * @brief Resize the GUI renderer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            DeleteFramebuffer();
            (void) GenerateFramebuffer(width, height);
            guiDisplay.Resize(width, height);
        }

        /**
         * @brief Update the GUI renderer.
         * @param[in] dt The elapsed time to the previous update event in seconds.
         * @details This member function is called before @ref Draw.
         */
        void Update(double dt){
            guiDisplay.Update(dt);
        }

        /**
         * @brief Use the GUI renderer.
         */
        void Use(void){
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            DEBUG_GLCHECK( glViewport(0, 0, width, height); );
            DEBUG_GLCHECK( glCullFace(GL_BACK); );
            DEBUG_GLCHECK( glDisable(GL_STENCIL_TEST); );
            DEBUG_GLCHECK( glDisable(GL_DEPTH_TEST); );
            DEBUG_GLCHECK( glDisable(GL_BLEND); );
            DEBUG_GLCHECK( glEnable(GL_BLEND); );
            DEBUG_GLCHECK( glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); );
            DEBUG_GLCHECK( glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD); );
            DEBUG_GLCHECK( glClearColor(0.0f, 0.0f, 0.0f, 0.0f); );
            DEBUG_GLCHECK( glClear(GL_COLOR_BUFFER_BIT); );
        }

        /**
         * @brief Draw the GUI.
         * @param[in] manualControlEnabled True if manual camera control is enabled, false otherwise.
         */
        void Draw(bool manualControlEnabled){
            guiDisplay.Draw(manualControlEnabled);
        }

        /**
         * @brief Get the GUI texture.
         * @return Texture ID of the GUI colorbuffer.
         */
        GLuint GetGUITexture(void){ return cbo; }

    private:
        GLsizei width;              // The width of the framebuffer in pixels.
        GLsizei height;             // The height of the framebuffer in pixels.
        GLuint cbo;                 // Colorbuffer (RGBA).
        GLuint rbo;                 // Renderbuffer object.
        GLuint fbo;                 // The actual framebuffer object.
        GUIDisplay guiDisplay;      // The GUI display.

        /**
         * @brief Generate colorbuffer, renderbuffer and framebuffer.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool GenerateFramebuffer(GLsizei width, GLsizei height){
            DEBUG_GLCHECK( glGenFramebuffers(1, &fbo); );
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbo); );
            // colorbuffer
            DEBUG_GLCHECK( glGenTextures(1, &cbo); );
            DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, cbo); );
            DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); );
            DEBUG_GLCHECK( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cbo, 0); );
            // rendering buffer for depth
            DEBUG_GLCHECK( glGenRenderbuffers(1, &rbo); );
            DEBUG_GLCHECK( glBindRenderbuffer(GL_RENDERBUFFER, rbo);  );
            DEBUG_GLCHECK( glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); );
            DEBUG_GLCHECK( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); );
            // check for completeness
            if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)){
                PrintE("Framebuffer for GUI is not complete!\n");
                DeleteFramebuffer();
                return false;
            }
            DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0); );
            this->width = width;
            this->height = height;
            return true;
        }

        /**
         * @brief Delete colorbuffers, renderbuffer and framebuffer object.
         */
        void DeleteFramebuffer(void){
            if(cbo){
                DEBUG_GLCHECK( glDeleteTextures(1, &cbo); );
                cbo = 0;
            }
            if(rbo){
                DEBUG_GLCHECK( glDeleteRenderbuffers(1, &rbo); );
                rbo = 0;
            }
            if(fbo){
                DEBUG_GLCHECK( glDeleteFramebuffers(1, &fbo); );
                fbo = 0;
            }
            width = 0;
            height = 0;
        }
};

