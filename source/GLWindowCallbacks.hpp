#pragma once


#include <Common.hpp>


/* Forward declaration */
class GLWindow;


class GLWindowCallbacks {
    public:
        /**
         * @brief Destroy the GL window callbacks object.
         */
        virtual ~GLWindowCallbacks(){}

    protected:
        friend GLWindow;

        /**
         * @brief The initialization callback function.
         * @param[in] wnd The GLFW window.
         * @return True if success, false otherwise.
         * @details This function is called before the rendering loop.
         */
        virtual bool CallbackInitialize(GLFWwindow* wnd){
            (void)wnd;
            return true;
        }

        /**
         * @brief The termination callback function.
         * @param[in] wnd The GLFW window.
         * @details This function is called after the rendering loop.
         */
        virtual void CallbackTerminate(GLFWwindow* wnd){
            (void)wnd;
        }

        /**
         * @brief The update callback function.
         * @param[in] wnd The GLFW window.
         * @param[in] dt The elapsed time to the previous update event in seconds.
         * @details This callback function is called before @ref CallbackRender.
         */
        virtual void CallbackUpdate(GLFWwindow* wnd, double dt){
            (void)wnd;
            (void)dt;
        }

        /**
         * @brief The rendering callback function.
         * @param[in] wnd The GLFW window.
         * @param[in] time The current rendering time.
         */
        virtual void CallbackRender(GLFWwindow* wnd){
            (void)wnd;
        }

        /**
         * @brief The GLFW key callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] key The keyboard key that was pressed or released.
         * @param[in] scancode The system-specific scancode of the key.
         * @param[in] action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions.
         * @param[in] mods Bit field describing which modifier keys were held down.
         */
        virtual void CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){
            (void)wnd;
            (void)key;
            (void)scancode;
            (void)action;
            (void)mods;
        }

        /**
         * @brief The GLFW mouse button callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] button The mouse button that was pressed or released.
         * @param[in] action One of GLFW_PRESS or GLFW_RELEASE. Future releases may add more actions.
         * @param[in] mods Bit field describing which modifier keys were held down.
         */
        virtual void CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
            (void)wnd;
            (void)button;
            (void)action;
            (void)mods;
        }

        /**
         * @brief The GLFW cursor position callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] xPos The new cursor x-coordinate, relative to the left edge of the content area.
         * @param[in] yPos The new cursor y-coordinate, relative to the top edge of the content area.
         */
        virtual void CallbackCursorPosition(GLFWwindow* wnd, double xPos, double yPos){
            (void)wnd;
            (void)xPos;
            (void)yPos;
        }

        /**
         * @brief The GLFW scroll callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] xoffset The scroll offset along the x-axis.
         * @param[in] yoffset The scroll offset along the y-axis.
         */
        virtual void CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset){
            (void)wnd;
            (void)xoffset;
            (void)yoffset;
        }

        /**
         * @brief The GLFW framebuffer size callback function.
         * @param[in] wnd The window whose framebuffer was resized.
         * @param[in] width The new width, in pixels, of the framebuffer.
         * @param[in] height The new height, in pixels, of the framebuffer.
         */
        virtual void CallbackFramebufferSize(GLFWwindow* wnd, int width, int height){
            (void)wnd;
            (void)width;
            (void)height;
        }
};

