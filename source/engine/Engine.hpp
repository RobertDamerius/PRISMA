#pragma once
// #define DEBUG_PRINT_PERFORMANCE // uncomment if required


#include <GLWindowCallbacks.hpp>
#include <InputSystem.hpp>
#include <PrismaState.hpp>
#include <GBuffer.hpp>
#include <Environment.hpp>
#include <MinimalEnvironment.hpp>
#include <Bloom.hpp>
#include <CascadedShadowMapper.hpp>
#include <AmbientOcclusion.hpp>
#include <GUIRenderer.hpp>
#include <PostProcessing.hpp>
#ifdef DEBUG_PRINT_PERFORMANCE
#include <GLPerformanceCounter.hpp>
#endif


class Engine: public GLWindowCallbacks {
    protected:
        /**
         * @brief The initialization callback function.
         * @param[in] wnd The GLFW window.
         * @return True if success, false otherwise.
         * @details This function is called before the rendering loop.
         */
        bool CallbackInitialize(GLFWwindow* wnd);

        /**
         * @brief The termination callback function.
         * @param[in] wnd The GLFW window.
         * @details This function is called after the rendering loop.
         */
        void CallbackTerminate(GLFWwindow* wnd);

        /**
         * @brief The update callback function.
         * @param[in] wnd The GLFW window.
         * @param[in] dt The elapsed time to the previous update event in seconds.
         * @details This callback function is called before @ref CallbackRender.
         */
        void CallbackUpdate(GLFWwindow* wnd, double dt);

        /**
         * @brief The rendering callback function.
         * @param[in] wnd The GLFW window.
         */
        void CallbackRender(GLFWwindow* wnd);

        /**
         * @brief The GLFW framebuffer size callback function.
         * @param[in] wnd The window whose framebuffer was resized.
         * @param[in] width The new width, in pixels, of the framebuffer.
         * @param[in] height The new height, in pixels, of the framebuffer.
         */
        void CallbackFramebufferSize(GLFWwindow* wnd, int width, int height);

        /**
         * @brief The GLFW key callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] key The keyboard key that was pressed or released.
         * @param[in] scancode The system-specific scancode of the key.
         * @param[in] action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions.
         * @param[in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods);

        /**
         * @brief The GLFW cursor position callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] xPos The new cursor x-coordinate, relative to the left edge of the content area.
         * @param[in] yPos The new cursor y-coordinate, relative to the top edge of the content area.
         */
        void CallbackCursorPosition(GLFWwindow* wnd, double xPos, double yPos);

        /**
         * @brief The GLFW mouse button callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] button The mouse button that was pressed or released.
         * @param[in] action One of GLFW_PRESS or GLFW_RELEASE. Future releases may add more actions.
         * @param[in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods);

        /**
         * @brief The GLFW scroll callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] xoffset The scroll offset along the x-axis.
         * @param[in] yoffset The scroll offset along the y-axis.
         */
        void CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset);

    private:
        InputSystem inputSystem;                     // The input system that handles events from human interface devices (keyboard, mouse).
        PrismaState prismaState;                     // The PRISMA state that contains all settings and objects to be rendered.
        GBuffer gBuffer;                             // The G-buffer to be used for deferred rendering of the scene.
        Environment environment;                     // The environment renderer.
        MinimalEnvironment environmentReflection;    // The reflection environment renderer.
        MinimalEnvironment environmentRefraction;    // The refraction environment renderer.
        Bloom bloom;                                 // The bloom renderer.
        CascadedShadowMapper shadowMapper;           // The cascaded shadow mapper.
        AmbientOcclusion ambientOcclusion;           // The ambient occlusion renderer.
        PostProcessing postProcessing;               // The post processing renderer.
        GUIRenderer guiRenderer;                     // The GUI renderer.
        #ifdef DEBUG_PRINT_PERFORMANCE
        GLPerformanceCounter glPerformanceCounter;   // A query counter to be used to measure performance of draw calls.
        #endif

        /**
         * @brief Clear double buffers and set default color (black)
         * @param[in] wnd The window for which to clear double buffers.
         */
        void ClearDoubleBuffers(GLFWwindow* wnd);

        /**
         * @brief Render the whole scene (excluding water) from two points of view to obtain reflection and refraction textures.
         */
        void RenderSceneReflectionAndRefraction(void);
};

