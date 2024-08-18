#include <GLWindow.hpp>
#include <PrismaConfiguration.hpp>


GLWindow::GLWindow(){
    glfwWindow = nullptr;
}

GLWindow::~GLWindow(){
    Terminate();
}

void GLWindow::Run(GLWindowCallbacks* callbacks){
    if(callbacks){
        if(Initialize(callbacks)){
            MainLoop(callbacks);
            Terminate();
        }
    }
}

void GLWindow::Close(void){
    if(glfwWindow){
        glfwSetWindowShouldClose(glfwWindow, GL_TRUE);
    }
}

bool GLWindow::Initialize(GLWindowCallbacks* callbacks){
    // Initialize GLFW
    glfwSetErrorCallback(GLWindow::CallbackGLFWError);
    if(GL_TRUE != glfwInit()){
        PrintE("Could not initialize GLFW!\n");
        return false;
    }
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, prismaConfiguration.window.glMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, prismaConfiguration.window.glMinorVersion);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE); // enable double-buffering
    glfwWindowHint(GLFW_SAMPLES, 0); // disable multi-sampling
    glfwWindowHint(GLFW_RESIZABLE, prismaConfiguration.window.resizable ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, prismaConfiguration.window.visible ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, prismaConfiguration.window.decorated ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_FOCUSED, prismaConfiguration.window.focused ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, prismaConfiguration.window.maximized ? GL_TRUE : GL_FALSE);

    // select monitor
    int numberOfDetectedMonitors = 0;
    GLFWmonitor** detectedMonitors = glfwGetMonitors(&numberOfDetectedMonitors);
    GLFWmonitor* targetMonitor = nullptr;
    if(prismaConfiguration.window.fullscreen && detectedMonitors && (numberOfDetectedMonitors > 0)){
        uint32_t monitorIndex = std::min(prismaConfiguration.window.fullscreenMonitorIndex, static_cast<uint32_t>(numberOfDetectedMonitors - 1));
        targetMonitor = detectedMonitors[monitorIndex];
    }

    // Create Window
    glfwWindow = glfwCreateWindow(prismaConfiguration.window.width, prismaConfiguration.window.height, prismaConfiguration.window.title.c_str(), targetMonitor, nullptr); // nullptr: no shared resources
    if(!glfwWindow){
        PrintE("Could not create GLFW window!\n");
        glfwTerminate();
        return false;
    }
    glfwSetWindowUserPointer(glfwWindow, reinterpret_cast<void*>(callbacks));

    // Set callbacks
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* wnd, int key, int scancode, int action, int mods){
        reinterpret_cast<GLWindowCallbacks*>(glfwGetWindowUserPointer(wnd))->CallbackKey(wnd, key, scancode, action, mods);
    });
    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* wnd, int button, int action, int mods){
        reinterpret_cast<GLWindowCallbacks*>(glfwGetWindowUserPointer(wnd))->CallbackMouseButton(wnd, button, action, mods);
    });
    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* wnd, double xPos, double yPos){
        reinterpret_cast<GLWindowCallbacks*>(glfwGetWindowUserPointer(wnd))->CallbackCursorPosition(wnd, xPos, yPos);
    });
    glfwSetScrollCallback(glfwWindow, [](GLFWwindow* wnd, double xoffset, double yoffset){
        reinterpret_cast<GLWindowCallbacks*>(glfwGetWindowUserPointer(wnd))->CallbackScroll(wnd, xoffset, yoffset);
    });
    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* wnd, int width, int height){
        reinterpret_cast<GLWindowCallbacks*>(glfwGetWindowUserPointer(wnd))->CallbackFramebufferSize(wnd, width, height);
    });

    // Set OpenGL context
    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1);
    PrintGLInfo();

    // Initiate GLEW
    glewExperimental = GL_TRUE;
    if(GLEW_OK != glewInit()){
        PrintE("Could not initialize GLEW!\n");
        glfwDestroyWindow(glfwWindow);
        glfwWindow = nullptr;
        glfwTerminate();
        return false;
    }
    return true;
}

void GLWindow::Terminate(void){
    if(glfwWindow){
        glfwDestroyWindow(glfwWindow);
        glfwWindow = nullptr;
    }
    glfwTerminate();
}

void GLWindow::MainLoop(GLWindowCallbacks* callbacks){
    if(glfwWindow){
        if(callbacks->CallbackInitialize(glfwWindow)){
            glfwSetTime(0.0);
            double tPrevious = 0.0;
            double tFPS = 0.0;
            while(!glfwWindowShouldClose(glfwWindow)){
                // measure elapsed time
                double tNow = glfwGetTime();
                double dt = tNow - tPrevious;
                tPrevious = tNow;

                // show FPS in window title
                tFPS += dt;
                if(tFPS >= 0.2){
                    int32_t fps = static_cast<int32_t>(std::round(1.0 / dt));
                    tFPS = std::fmod(tFPS, 0.2);
                    if(prismaConfiguration.window.showFPSInTitle){
                        std::string sFPS = prismaConfiguration.window.title + " (" + std::to_string(fps) + " FPS)";
                        glfwSetWindowTitle(glfwWindow, sFPS.c_str());
                    }
                }

                // update, render and swap double buffers
                callbacks->CallbackUpdate(glfwWindow, dt);
                callbacks->CallbackRender(glfwWindow);
                glfwSwapBuffers(glfwWindow);
                glfwPollEvents();
            }
        }
        callbacks->CallbackTerminate(glfwWindow);
    }
}

void GLWindow::PrintGLInfo(void){
    GLint iMajor, iMinor;
    const GLubyte *strGLSL = glGetString(GL_SHADING_LANGUAGE_VERSION);
    const GLubyte *strVendor = glGetString(GL_VENDOR);
    const GLubyte *strRenderer = glGetString(GL_RENDERER);
    DEBUG_GLCHECK( glGetIntegerv(GL_MAJOR_VERSION, &iMajor); );
    DEBUG_GLCHECK( glGetIntegerv(GL_MINOR_VERSION, &iMinor); );
    if(strGLSL && strVendor && strRenderer){
        Print("GL Version:               %d.%d\n", iMajor, iMinor);
        Print("GLSL Version:             %s\n", (const char*)strGLSL);
        Print("GL Vendor:                %s\n", (const char*)strVendor);
        Print("GL Renderer:              %s\n\n", (const char*)strRenderer);
    }
}

void GLWindow::CallbackGLFWError(int error, const char* description){
    PrintE("GLFW ERROR %d: %s\n", error, description);
}

