#pragma once


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// External Libraries
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* Default C/C++ headers */
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <cstdarg>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <iterator>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <random>
#include <algorithm>


/* OS depending */
// Windows System
#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Iphlpapi.h>
// Unix System
#elif __linux__
#include <execinfo.h>
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <net/if.h>
#else
// Other
#endif

/* OpenGL GLAD/GLFW/GLM */
#include <glad/glad.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Version Settings
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern const char* const PRISMA_VERSION;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Macros For Thread-safe Console Prints
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern std::mutex __prisma_mtx_print;
extern void __prisma_print(const char* format, ...);
extern void __prisma_print_verbose(const char* prefix, const char* file, const int line, const char* func, const char* format, ...);


#define Print(...) __prisma_print(__VA_ARGS__)
#define PrintW(...) __prisma_print_verbose("WARNING", __FILE__, __LINE__, __func__, __VA_ARGS__)
#define PrintE(...) __prisma_print_verbose("ERROR", __FILE__, __LINE__, __func__, __VA_ARGS__)


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Debug macros for GL
// Example: DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, 0); );
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern std::string GLErrorToString(GLenum error);


#ifdef DEBUG
#define DEBUG_GLCHECK(stmt)    \
    do { \
        GLenum e = glGetError(); \
        stmt; \
        if(GL_NO_ERROR != (e = glGetError())){ \
            std::lock_guard<std::mutex> lk(__prisma_mtx_print); \
            fprintf(stderr,"GL_ERROR %s:%d in %s() \"%s\": %s\n", __FILE__, __LINE__, __func__, #stmt, GLErrorToString(e).c_str()); \
        } \
    } while(0)
#else
#define DEBUG_GLCHECK(stmt) stmt
#endif /* DEBUG */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Uniform buffer objects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define UBO_CAMERA                               (1)
#define UBO_LIGHTMATRICES                        (2)
#define UBO_ATMOSPHERE                           (3)


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Shader storage buffer objects
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SSBO_DIRECTIONALLIGHTS                   (1)
#define SSBO_POINTLIGHTS                         (2)
#define SSBO_SPOTLIGHTS                          (3)

