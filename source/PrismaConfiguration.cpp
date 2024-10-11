#include <PrismaConfiguration.hpp>
#include <FileManager.hpp>
#include <nlohmann/json.hpp>


PrismaConfiguration prismaConfiguration;


PrismaConfiguration::PrismaConfiguration(){
    Clear();
}

void PrismaConfiguration::Clear(void){
    window.width = 1920;
    window.height = 1080;
    window.fullscreen = false;
    window.fullscreenMonitorIndex = 0;
    window.resizable = true;
    window.visible = true;
    window.decorated = true;
    window.focused = true;
    window.maximized = false;
    window.glMajorVersion = 4;
    window.glMinorVersion = 5;
    window.title = "PRISMA";
    window.showFPSInTitle = false;
    window.closeWithEscape = true;
    window.allowManualCameraControl = true;
    engine.enableWaterReflection = true;
    engine.enableAtmosphericScattering = true;
    engine.enableShadowMapping = true;
    engine.enableAmbientOcclusion = true;
    engine.enableBloom = true;
    engine.maxNumDirectionalLights = 8;
    engine.maxNumPointLights = 256;
    engine.maxNumSpotLights = 256;
    engine.ssaoKernelSize = 16;
    engine.shadowFrustumSplitLevels = {0.04f, 0.1f, 0.5f};
    engine.shadowMapResolution = 4096;
    engine.maxNumBloomMips = 6;
    network.localPort = 31416;
    network.interfaceAddress = {0,0,0,0};
    network.interfaceName = "";
    network.socketPriority = 6;
    network.socketErrorRetryTimeMs = 1000;
    network.receive.multicastGroups = {{239,0,0,1}};
    network.send.destinationAddresses.clear();
    network.send.timeToLive = 1;
}

bool PrismaConfiguration::ReadFromFile(std::string alternativeConfigurationFile){
    // set default values
    Clear();

    // parse JSON file
    nlohmann::json jsonData;
    std::string filename = FileName(FILENAME_PRISMA_CONFIGURATION);
    if(!alternativeConfigurationFile.empty()){
        filename = alternativeConfigurationFile;
    }
    try{
        std::ifstream configurationFile(filename);
        originalFileContent.assign(std::istreambuf_iterator<char>(configurationFile), std::istreambuf_iterator<char>());
        configurationFile.seekg(0, std::ios::beg);
        jsonData = nlohmann::json::parse(configurationFile);
    }
    catch(const std::exception& e){
        PrintE("Failed to parse configuration file \"%s\": %s\n", filename.c_str(), e.what());
        return false;
    }

    // set configuration values if possible
    bool success = true;
    try{ window.width                       = jsonData.at("window").at("width");                                                } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.height                      = jsonData.at("window").at("height");                                               } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.fullscreen                  = jsonData.at("window").at("fullscreen");                                           } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.fullscreenMonitorIndex      = jsonData.at("window").at("fullscreenMonitorIndex");                               } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.resizable                   = jsonData.at("window").at("resizable");                                            } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.visible                     = jsonData.at("window").at("visible");                                              } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.decorated                   = jsonData.at("window").at("decorated");                                            } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.focused                     = jsonData.at("window").at("focused");                                              } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.maximized                   = jsonData.at("window").at("maximized");                                            } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.glMajorVersion              = jsonData.at("window").at("glMajorVersion");                                       } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.glMinorVersion              = jsonData.at("window").at("glMinorVersion");                                       } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.title                       = jsonData.at("window").at("title");                                                } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.showFPSInTitle              = jsonData.at("window").at("showFPSInTitle");                                       } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.closeWithEscape             = jsonData.at("window").at("closeWithEscape");                                      } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ window.allowManualCameraControl    = jsonData.at("window").at("allowManualCameraControl");                             } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.enableWaterReflection       = jsonData.at("engine").at("enableWaterReflection");                                } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.enableAtmosphericScattering = jsonData.at("engine").at("enableAtmosphericScattering");                          } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.enableShadowMapping         = jsonData.at("engine").at("enableShadowMapping");                                  } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.enableAmbientOcclusion      = jsonData.at("engine").at("enableAmbientOcclusion");                               } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.enableBloom                 = jsonData.at("engine").at("enableBloom");                                          } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.maxNumDirectionalLights     = jsonData.at("engine").at("maxNumDirectionalLights");                              } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.maxNumPointLights           = jsonData.at("engine").at("maxNumPointLights");                                    } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.maxNumSpotLights            = jsonData.at("engine").at("maxNumSpotLights");                                     } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.ssaoKernelSize              = jsonData.at("engine").at("ssaoKernelSize");                                       } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.shadowFrustumSplitLevels    = jsonData.at("engine").at("shadowFrustumSplitLevels").get<std::vector<GLfloat>>(); } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.shadowMapResolution         = jsonData.at("engine").at("shadowMapResolution");                                  } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ engine.maxNumBloomMips             = jsonData.at("engine").at("maxNumBloomMips");                                      } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ network.localPort                  = jsonData.at("network").at("localPort");                                           } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ network.interfaceAddress           = jsonData.at("network").at("interfaceAddress");                                    } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ network.interfaceName              = jsonData.at("network").at("interfaceName");                                       } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ network.socketPriority             = jsonData.at("network").at("socketPriority");                                      } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ network.socketErrorRetryTimeMs     = jsonData.at("network").at("socketErrorRetryTimeMs");                              } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ network.receive.multicastGroups    = jsonData.at("network").at("receive").at("multicastGroups");                       } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ network.send.destinationAddresses  = jsonData.at("network").at("send").at("destinationAddresses");                     } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    try{ network.send.timeToLive            = jsonData.at("network").at("send").at("timeToLive");                               } catch(const std::exception& e){ success = false; PrintE("Error in configuration file \"%s\": %s\n", filename.c_str(), e.what()); }
    for(auto&& a : network.send.destinationAddresses){
        if((a[0] > 0x00FF) || (a[1] > 0x00FF) || (a[2] > 0x00FF) || (a[3] > 0x00FF) || !a[4]){
            PrintE("Error in configuration file \"%s\": Invalid destination address %u.%u.%u.%u:%u!\n", filename.c_str(), a[0], a[1], a[2], a[3], a[4]);
            success = false;
        }
    }
    return success;
}

std::string PrismaConfiguration::GetOriginalFileContent(void){
    return originalFileContent;
}

