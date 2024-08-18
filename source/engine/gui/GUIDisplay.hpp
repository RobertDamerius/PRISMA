#pragma once


#include <Common.hpp>
#include <ImageTexture2D.hpp>
#include <FileManager.hpp>
#include <ScreenQuad.hpp>
#include <ShaderGUIImage.hpp>


class GUIDisplay {
    public:
        /**
         * @brief Generate the GUI display.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         * @return True if success, false otherwise.
         */
        bool Generate(GLsizei width, GLsizei height){
            bool success = true;
            SetImageTransformations(width, height);
            screenQuad.Generate();
            success &= splashScreen.ReadFromPngFile(FileName(FILENAME_TEXTURE_SPLASHSCREEN), true);
            success &= splashScreen.Generate(GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR, false, false, false);
            success &= manualControl.ReadFromPngFile(FileName(FILENAME_TEXTURE_MANUALCONTROL), true);
            success &= manualControl.Generate(GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR, false, false, false);
            success &= imageShader.Generate();
            if(!success){
                PrintE("Could not generate GUI display!\n");
                Delete();
            }
            animationTime = 0.0;
            return success;
        }

        /**
         * @brief Delete the GUI display.
         */
        void Delete(void){
            splashScreen.Delete();
            imageShader.Delete();
            screenQuad.Delete();
        }

        /**
         * @brief Resize the GUI display.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            SetImageTransformations(width, height);
        }

        /**
         * @brief Update the GUI display.
         * @param[in] dt The elapsed time to the previous update event in seconds.
         * @details This member function is called before @ref Draw.
         */
        void Update(double dt){
            animationTime += dt;
        }

        /**
         * @brief Draw the display.
         * @param[in] manualControlEnabled True if manual camera control is enabled, false otherwise.
         */
        void Draw(bool manualControlEnabled){
            bool drawSplashScreen = (splashScreenImageTransform.z < 1.0f) && (splashScreenImageTransform.w < 1.0f) && (animationTime < 2.0);
            DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE0); );
            imageShader.Use();
            if(drawSplashScreen){
                double alpha = std::clamp((1.6 - animationTime)*5.0, 0.0, 1.0);
                imageShader.SetImageTransform(splashScreenImageTransform);
                imageShader.SetImageColorScale(glm::vec4(1.0f, 1.0f, 1.0f, static_cast<GLfloat>(alpha)));
                splashScreen.BindTexture();
                screenQuad.Draw();
            }
            if(manualControlEnabled){
                imageShader.SetImageTransform(manualControlImageTransform);
                imageShader.SetImageColorScale(glm::vec4(1.0f));
                manualControl.BindTexture();
                screenQuad.Draw();
            }
        }

    private:
        ScreenQuad screenQuad;                   // The screen quad to be used for rendering.
        ShaderGUIImage imageShader;              // The image shader.
        double animationTime;                    // The total time starting from zero.

        /* splash screen */
        ImageTexture2D splashScreen;             // The splash screen image.
        glm::vec4 splashScreenImageTransform;    // Image transformation for the splash screen image (position.x, position.y, scale.x, scale.y).

        /* manual control */
        ImageTexture2D manualControl;            // The manual control image.
        glm::vec4 manualControlImageTransform;   // Image transformation for the manual-control image (position.x, position.y, scale.x, scale.y).

        /**
         * @brief Set the image transformations.
         * @param[in] width Width of the framebuffer in pixels.
         * @param[in] height Height of the framebuffer in pixels.
         */
        void SetImageTransformations(GLsizei width, GLsizei height){
            // splash screen image
            double sx = static_cast<double>(splashScreen.GetWidth()) / static_cast<double>(width);
            double sy = static_cast<double>(splashScreen.GetHeight()) / static_cast<double>(height);
            splashScreenImageTransform.x = 0.0f;
            splashScreenImageTransform.y = 0.0f;
            splashScreenImageTransform.z = static_cast<GLfloat>(sx);
            splashScreenImageTransform.w = static_cast<GLfloat>(sy);

            // manual control image
            sx = static_cast<double>(manualControl.GetWidth()) / static_cast<double>(width);
            sy = static_cast<double>(manualControl.GetHeight()) / static_cast<double>(height);
            manualControlImageTransform.x = static_cast<GLfloat>(1.0 - sx * 2.0);
            manualControlImageTransform.y = static_cast<GLfloat>(-1.0 + sy * 2.0);
            manualControlImageTransform.z = static_cast<GLfloat>(sx);
            manualControlImageTransform.w = static_cast<GLfloat>(sy);
        }
};

