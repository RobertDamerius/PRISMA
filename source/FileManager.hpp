#pragma once


#include <Common.hpp>


namespace FileManager {


/**
 * @brief Get the absolute path to the application directory.
 * @return Absolute path to the application directory.
 */
inline std::filesystem::path GetApplicationPath(void){
    #ifdef _WIN32
    char* buffer = new char[65536];
    DWORD len = GetModuleFileNameA(NULL, &buffer[0], 65536);
    std::string str(buffer, len);
    delete[] buffer;
    #else
    std::string str("/proc/self/exe");
    #endif
    std::filesystem::path applicationPath;
    try {
        applicationPath = std::filesystem::canonical(str);
        applicationPath.remove_filename();
    }
    catch(...){ }
    return applicationPath;
}


/**
 * @brief Generate the filename of the protocol file based on the current system time.
 * @return The filename of the format "YYYYMMDD_hhmmssmmm.txt".
 */
inline std::string GenerateProtocolFileName(void){
    auto timePoint = std::chrono::system_clock::now();
    std::time_t systemTime = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* gmTime = std::gmtime(&systemTime);
    uint32_t utcYear = static_cast<uint32_t>(gmTime->tm_year);
    uint32_t utcMonth = static_cast<uint32_t>(gmTime->tm_mon);
    uint32_t utcMDay = static_cast<uint32_t>(gmTime->tm_mday);
    uint32_t utcHour = static_cast<uint32_t>(gmTime->tm_hour);
    uint32_t utcMinute = static_cast<uint32_t>(gmTime->tm_min);
    uint32_t utcSecond = static_cast<uint32_t>(gmTime->tm_sec);
    auto duration = timePoint.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    uint32_t utcNanoseconds = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
    char buffer[64];
    sprintf(buffer, "%u%02u%02u_%02u%02u%02u%03u", 1900 + utcYear, 1 + utcMonth, utcMDay, utcHour, utcMinute, utcSecond, utcNanoseconds / 1000000);
    return std::string(buffer) + ".txt";
}


} /* namespace: FileManager */


enum EnumFileName {
    FILENAME_PRISMA_PROTOCOL,
    FILENAME_PRISMA_CONFIGURATION,
    FILENAME_SHADER_COLORMESH,
    FILENAME_SHADER_TEXTUREMESH,
    FILENAME_SHADER_MESHSHADOWDEPTH,
    FILENAME_SHADER_ALPHAMESHSHADOWDEPTH,
    FILENAME_SHADER_WATERMESH,
    FILENAME_SHADER_DYNAMICMESH,
    FILENAME_SHADER_SSAO,
    FILENAME_SHADER_ENVIRONMENT,
    FILENAME_SHADER_MINIMAL_ENVIRONMENT,
    FILENAME_SHADER_BLOOMDOWNSAMPLE,
    FILENAME_SHADER_BLOOMUPSAMPLE,
    FILENAME_SHADER_POSTPROCESSING,
    FILENAME_SHADER_GUIIMAGE,
    FILENAME_TEXTURE_WATER_DUDVMAP,
    FILENAME_TEXTURE_WATER_NORMALMAP,
    FILENAME_TEXTURE_SPLASHSCREEN,
    FILENAME_TEXTURE_MANUALCONTROL,
    FILENAME_DIRECTORY_PROTOCOL,
    FILENAME_DIRECTORY_MESH,
    FILENAME_DIRECTORY_TEXTURE
};


/**
 * @brief Get the absolute filename for a specific file.
 * @param[in] enumFileName Enumeration file that specifies the file for which to obtain the absolute filename.
 * @return Absolute filename.
 */
inline std::string FileName(EnumFileName enumFileName){
    std::filesystem::path applicationPath = FileManager::GetApplicationPath();
    std::filesystem::path protocolPath = applicationPath / "protocol";
    std::filesystem::path shaderPath = applicationPath / "assets" / "shader";
    std::filesystem::path meshPath = applicationPath / "assets" / "mesh";
    std::filesystem::path texturePath = applicationPath / "assets" / "texture";
    std::filesystem::path result;
    switch(enumFileName){
        case FILENAME_PRISMA_PROTOCOL:               result = protocolPath / FileManager::GenerateProtocolFileName();   break;
        case FILENAME_PRISMA_CONFIGURATION:          result = applicationPath / "assets" / "PRISMA.json";               break;
        case FILENAME_SHADER_COLORMESH:              result = shaderPath / "ColorMesh.glsl";                            break;
        case FILENAME_SHADER_TEXTUREMESH:            result = shaderPath / "TextureMesh.glsl";                          break;
        case FILENAME_SHADER_MESHSHADOWDEPTH:        result = shaderPath / "MeshShadowDepth.glsl";                      break;
        case FILENAME_SHADER_ALPHAMESHSHADOWDEPTH:   result = shaderPath / "AlphaMeshShadowDepth.glsl";                 break;
        case FILENAME_SHADER_WATERMESH:              result = shaderPath / "WaterMesh.glsl";                            break;
        case FILENAME_SHADER_DYNAMICMESH:            result = shaderPath / "DynamicMesh.glsl";                          break;
        case FILENAME_SHADER_SSAO:                   result = shaderPath / "SSAO.glsl";                                 break;
        case FILENAME_SHADER_ENVIRONMENT:            result = shaderPath / "Environment.glsl";                          break;
        case FILENAME_SHADER_MINIMAL_ENVIRONMENT:    result = shaderPath / "MinimalEnvironment.glsl";                   break;
        case FILENAME_SHADER_BLOOMDOWNSAMPLE:        result = shaderPath / "BloomDownsample.glsl";                      break;
        case FILENAME_SHADER_BLOOMUPSAMPLE:          result = shaderPath / "BloomUpsample.glsl";                        break;
        case FILENAME_SHADER_POSTPROCESSING:         result = shaderPath / "PostProcessing.glsl";                       break;
        case FILENAME_SHADER_GUIIMAGE:               result = shaderPath / "GUIImage.glsl";                             break;
        case FILENAME_TEXTURE_WATER_DUDVMAP:         result = texturePath / "WaterDuDvMap.png";                         break;
        case FILENAME_TEXTURE_WATER_NORMALMAP:       result = texturePath / "WaterNormalMap.png";                       break;
        case FILENAME_TEXTURE_SPLASHSCREEN:          result = texturePath / "SplashScreen.png";                         break;
        case FILENAME_TEXTURE_MANUALCONTROL:         result = texturePath / "ManualControl.png";                        break;
        case FILENAME_DIRECTORY_PROTOCOL:            result = protocolPath;                                             break;
        case FILENAME_DIRECTORY_MESH:                result = meshPath;                                                 break;
        case FILENAME_DIRECTORY_TEXTURE:             result = texturePath;                                              break;
    }
    return result.string();
}

