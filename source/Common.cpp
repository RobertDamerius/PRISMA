#include <Common.hpp>


const char* const PRISMA_VERSION = "1.0.0";


std::mutex __prisma_mtx_print;


void __prisma_print(const char* format, ...){
    const std::lock_guard<std::mutex> lk(__prisma_mtx_print);
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}

void __prisma_print_verbose(const char* prefix, const char* file, const int line, const char* func, const char* format, ...){
    const std::lock_guard<std::mutex> lk(__prisma_mtx_print);
    fprintf(stderr,"%s %s:%d in %s(): ", prefix, file, line, func);
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}

std::string GLErrorToString(GLenum error){
    if(GL_NO_ERROR == error)                        return std::string("GL_NO_ERROR");
    if(GL_INVALID_ENUM == error)                    return std::string("GL_INVALID_ENUM");
    if(GL_INVALID_VALUE == error)                   return std::string("GL_INVALID_VALUE");
    if(GL_INVALID_OPERATION == error)               return std::string("GL_INVALID_OPERATION");
    if(GL_INVALID_FRAMEBUFFER_OPERATION == error)   return std::string("GL_INVALID_FRAMEBUFFER_OPERATION");
    if(GL_OUT_OF_MEMORY == error)                   return std::string("GL_OUT_OF_MEMORY");
    if(GL_STACK_UNDERFLOW == error)                 return std::string("GL_STACK_UNDERFLOW");
    if(GL_STACK_OVERFLOW == error)                  return std::string("GL_STACK_OVERFLOW");
    return std::string("UNKNOWN GL ERROR");
}

