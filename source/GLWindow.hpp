#pragma once


#include <Common.hpp>
#include <NonCopyable.hpp>
#include <GLWindowCallbacks.hpp>


class GLWindow: private NonCopyable {
    public:
        /**
         * @brief Construct a new GL window object.
         */
        GLWindow();

        /**
         * @brief Destroy the GL window object.
         */
        ~GLWindow();

        /**
         * @brief Create window and run event loop.
         * @param[in] callbacks The object which should receive the window events.
         */
        void Run(GLWindowCallbacks* callbacks);

        /**
         * @brief Inform this GL window to close.
         */
        void Close(void);

    private:
        GLFWwindow* glfwWindow;    /// The internal GLFW window object.

        /**
         * @brief Initialize the GL window.
         * @param[in] callbacks The object which should receive the window events.
         * @return True if success, false otherwise.
         */
        bool Initialize(GLWindowCallbacks* callbacks);

        /**
         * @brief Terminate the GL window.
         */
        void Terminate(void);

        /**
         * @brief Run the main loop of the GL window.
         * @param[in] callbacks The object which should receive the window events.
         * @details Make sure that you initialized the window using the @ref Initialize member function.
         */
        void MainLoop(GLWindowCallbacks* callbacks);

        /**
         * @brief Print OpenGL information.
         */
        void PrintGLInfo(void);

        /**
         * @brief The GLFW error callback function.
         * @param[in] error An error code. Future releases may add more error codes.
         * @param[in] description A UTF-8 encoded string describing the error.
         */
        static void CallbackGLFWError(int error, const char* description);
};

