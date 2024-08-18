#include <Engine.hpp>
#include <MessageManager.hpp>
#include <PrismaConfiguration.hpp>
#include <MainApplication.hpp>


bool Engine::CallbackInitialize(GLFWwindow* wnd){
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.Generate();
    #endif
    bool success = true;
    int w, h;
    glfwGetFramebufferSize(wnd, &w, &h);
    GLsizei fbWidth = static_cast<GLsizei>(w);
    GLsizei fbHeight = static_cast<GLsizei>(h);
    ClearDoubleBuffers(wnd);
    success &= gBuffer.Generate(fbWidth, fbHeight);
    success &= shadowMapper.Generate(prismaConfiguration.engine.shadowFrustumSplitLevels, UBO_LIGHTMATRICES, prismaConfiguration.engine.shadowMapResolution);
    success &= environment.Generate(fbWidth, fbHeight, shadowMapper.GetNumberOfCascades());
    success &= environmentReflection.Generate(fbWidth, fbHeight, shadowMapper.GetNumberOfCascades());
    success &= environmentRefraction.Generate(fbWidth, fbHeight, shadowMapper.GetNumberOfCascades());
    success &= bloom.Generate(fbWidth, fbHeight);
    success &= ambientOcclusion.Generate(fbWidth, fbHeight, prismaConfiguration.engine.ssaoKernelSize);
    success &= postProcessing.Generate(fbWidth, fbHeight);
    success &= guiRenderer.Generate(fbWidth, fbHeight);
    success &= prismaState.Generate();
    CallbackFramebufferSize(wnd, w, h);
    return success;
}

void Engine::CallbackTerminate(GLFWwindow* wnd){
    prismaState.Delete();
    postProcessing.Delete();
    ambientOcclusion.Delete();
    bloom.Delete();
    shadowMapper.Delete();
    environmentRefraction.Delete();
    environmentReflection.Delete();
    environment.Delete();
    gBuffer.Delete();
    guiRenderer.Delete();
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.Delete();
    #endif
    (void)wnd;
}

void Engine::CallbackUpdate(GLFWwindow* wnd, double dt){
    // publish event message
    WindowEventMessage msg = inputSystem.GenerateWindowEventMessage(wnd);
    mainApplication.SendWindowEventMessage(msg);

    // update the camera if manual camera control is enabled
    bool manualCameraControl = inputSystem.UpdateCamera(prismaState.camera, dt);

    // update the PRISMA state
    std::vector<MessageBaseType*> messages = messageManager.FetchMessages();
    prismaState.meshLibrary.StartProtection();
    for(auto&& msg : messages){
        switch(msg->messageType){
            case MESSAGE_TYPE_AMBIENT_LIGHT:
                prismaState.lightLibrary.ProcessAmbientLightMessage(*reinterpret_cast<AmbientLightMessage*>(msg));
                break;
            case MESSAGE_TYPE_ATMOSPHERE:
                prismaState.atmosphere.ProcessAtmosphereMessage(*reinterpret_cast<AtmosphereMessage*>(msg));
                break;
            case MESSAGE_TYPE_CAMERA:
                if(!manualCameraControl){
                    CameraMessage* cameraMessage = reinterpret_cast<CameraMessage*>(msg);
                    switch(cameraMessage->camera.mode){
                        case CAMERA_MODE_PERSPECTIVE:
                        case CAMERA_MODE_ORTHOGRAPHIC:
                            prismaState.camera.mode =  cameraMessage->camera.mode;
                            break;
                    }
                    prismaState.camera.position =  cameraMessage->camera.position;
                    prismaState.camera.quaternion =  cameraMessage->camera.quaternion;
                    prismaState.camera.clipNear =  cameraMessage->camera.clipNear;
                    prismaState.camera.clipFar =  cameraMessage->camera.clipFar;
                    prismaState.camera.orthographicLeft =  cameraMessage->camera.orthographicLeft;
                    prismaState.camera.orthographicRight =  cameraMessage->camera.orthographicRight;
                    prismaState.camera.orthographicBottom =  cameraMessage->camera.orthographicBottom;
                    prismaState.camera.orthographicTop =  cameraMessage->camera.orthographicTop;
                    prismaState.camera.perspectiveFieldOfView =  cameraMessage->camera.perspectiveFieldOfView;
                }
                break;
            case MESSAGE_TYPE_CLEAR:
                prismaState.Clear(!manualCameraControl);
                break;
            case MESSAGE_TYPE_DIRECTIONAL_LIGHT:
                prismaState.lightLibrary.ProcessDirectionalLightMessage(*reinterpret_cast<DirectionalLightMessage*>(msg));
                break;
            case MESSAGE_TYPE_ENGINE_PARAMETER:
                prismaState.engineParameter = reinterpret_cast<EngineParameterMessage*>(msg)->engineParameter;
                break;
            case MESSAGE_TYPE_MESH:
                prismaState.meshLibrary.ProcessMeshMessage(*reinterpret_cast<MeshMessage*>(msg));
                break;
            case MESSAGE_TYPE_DYNAMIC_MESH:
                prismaState.dynamicMeshLibrary.ProcessDynamicMeshMessage(*reinterpret_cast<DynamicMeshMessage*>(msg));
                break;
            case MESSAGE_TYPE_POINT_LIGHT:
                prismaState.lightLibrary.ProcessPointLightMessage(*reinterpret_cast<PointLightMessage*>(msg));
                break;
            case MESSAGE_TYPE_SPOT_LIGHT:
                prismaState.lightLibrary.ProcessSpotLightMessage(*reinterpret_cast<SpotLightMessage*>(msg));
                break;
            case MESSAGE_TYPE_WATER_MESH:
                prismaState.waterMeshRenderer.ProcessWaterMeshMessage(*reinterpret_cast<WaterMeshMessage*>(msg));
                break;
        }
        delete msg;
    }
    prismaState.meshLibrary.EndProtection();

    // update the GUI
    guiRenderer.Update(dt);
}

void Engine::CallbackRender(GLFWwindow* wnd){
    // render scene reflection/refraction textures
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    if(prismaConfiguration.engine.enableWaterReflection){
        RenderSceneReflectionAndRefraction(); // updates the camera uniform buffer object
    }
    else{
        prismaState.camera.UpdateUniformBufferObject();
    }
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t1 = 1000.0 * glPerformanceCounter.EndQuery();
    #endif

    // render 3D scene to G-Buffer
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    gBuffer.Use();
    gBuffer.ClearFramebuffer(prismaState.camera.clipFar);
    gBuffer.DrawScene(prismaState);
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t2 = 1000.0 * glPerformanceCounter.EndQuery();
    #endif

    // render 3D scene to shadow mapper
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    if(prismaConfiguration.engine.enableShadowMapping){
        shadowMapper.Use();
        shadowMapper.DrawScene(prismaState);
    }
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t3 = 1000.0 * glPerformanceCounter.EndQuery();
    #endif

    // ambient Occlusion
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    if(prismaConfiguration.engine.enableAmbientOcclusion){
        ambientOcclusion.Use();
        ambientOcclusion.SetPositionTexture(gBuffer.GetPositionTexture());
        ambientOcclusion.SetNormalTexture(gBuffer.GetNormalTexture());
        ambientOcclusion.Draw(prismaState.engineParameter);
    }
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t4 = 1000.0 * glPerformanceCounter.EndQuery();
    #endif

    // render water mesh to G-buffer
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    gBuffer.Use();
    gBuffer.DrawWaterMesh(prismaState, environmentReflection.GetSceneTexture(), environmentRefraction.GetSceneTexture());
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t5 = 1000.0 * glPerformanceCounter.EndQuery();
    #endif

    // environmental rendering (lighting, shadow casting, fog, ...)
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    environment.Use(prismaState.engineParameter);
    environment.SetShadowCascadeFarPlanes(shadowMapper.GetCascadeFarPlanes());
    environment.SetPositionTexture(gBuffer.GetPositionTexture());
    environment.SetNormalTexture(gBuffer.GetNormalTexture());
    environment.SetDiffuseTexture(gBuffer.GetDiffuseTexture());
    environment.SetEmissionReflectionTexture(gBuffer.GetEmissionReflectionTexture());
    environment.SetSpecularShininessTexture(gBuffer.GetSpecularShininessTexture());
    environment.SetFaceNormalTexture(gBuffer.GetFaceNormalTexture());
    environment.SetOcclusionTexture(ambientOcclusion.GetOcclusionTexture());
    environment.SetShadowMapTexture(shadowMapper.GetDepthTextureArray());
    environment.ApplyScene(prismaState);
    environment.Draw();
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t6 = 1000.0 * glPerformanceCounter.EndQuery();
    #endif

    // apply bloom to brightness texture
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    if(prismaConfiguration.engine.enableBloom){
        bloom.Use();
        bloom.Draw(environment.GetBrightnessTexture(), environment.GetBrightnessTextureSize());
    }
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t7 = 1000.0 * glPerformanceCounter.EndQuery();
    #endif

    // render GUI
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    guiRenderer.Use();
    guiRenderer.Draw(inputSystem.ManualCameraControlEnabled());
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t8 = 1000.0 * glPerformanceCounter.EndQuery();
    #endif

    // post processing
    #ifdef DEBUG_PRINT_PERFORMANCE
    glPerformanceCounter.BeginQuery();
    #endif
    postProcessing.Use();
    postProcessing.SetGUITexture(guiRenderer.GetGUITexture());
    postProcessing.SetSceneTexture(environment.GetSceneTexture());
    postProcessing.SetBloomTexture(bloom.GetBloomTexture());
    postProcessing.Draw(prismaState.engineParameter);
    #ifdef DEBUG_PRINT_PERFORMANCE
    double t9 = 1000.0 * glPerformanceCounter.EndQuery();
    fprintf(stderr,"[ms]: ReRe=%07.3f   GBuf=%07.3f   CSM=%07.3f   SSAO=%07.3f   GBuf=%07.3f   ENV=%07.3f   BLOOM=%07.3f   GUI=%07.3f   POST=%07.3f   SUM=%07.3f\n",t1,t2,t3,t4,t5,t6,t7,t8,t9,t1+t2+t3+t4+t5+t6+t7+t8+t9);
    #endif
    (void)wnd;
}

void Engine::CallbackFramebufferSize(GLFWwindow* wnd, int width, int height){
    GLsizei fbWidth = static_cast<GLsizei>(width);
    GLsizei fbHeight = static_cast<GLsizei>(height);
    if((fbWidth > 0) && (fbHeight > 0)){
        gBuffer.Resize(fbWidth, fbHeight);
        environment.Resize(fbWidth, fbHeight);
        environmentReflection.Resize(fbWidth, fbHeight);
        environmentRefraction.Resize(fbWidth, fbHeight);
        prismaState.Resize(fbWidth, fbHeight);
        bloom.Resize(fbWidth, fbHeight);
        ambientOcclusion.Resize(fbWidth, fbHeight);
        guiRenderer.Resize(fbWidth, fbHeight);
        postProcessing.Resize(fbWidth, fbHeight);
    }
    (void)wnd;
}

void Engine::CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){
    inputSystem.CallbackKey(wnd, key, scancode, action, mods);
}

void Engine::CallbackCursorPosition(GLFWwindow* wnd, double xPos, double yPos){
    inputSystem.CallbackCursorPosition(wnd, xPos, yPos);
}

void Engine::CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
    inputSystem.CallbackMouseButton(wnd, button, action, mods);
}

void Engine::CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset){
    inputSystem.CallbackScroll(wnd, xoffset, yoffset);
}

void Engine::ClearDoubleBuffers(GLFWwindow* wnd){
    DEBUG_GLCHECK( glClearColor(0.0f, 0.0f, 0.0f, 0.0f); );
    DEBUG_GLCHECK( glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); );
    glfwSwapBuffers(wnd);
    DEBUG_GLCHECK( glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); );
    glfwSwapBuffers(wnd);
}

void Engine::RenderSceneReflectionAndRefraction(void){
    // get clipping plane for reflection and refraction depending on camera position
    glm::vec4 clipPlaneReflection = prismaState.waterMeshRenderer.GetClippingPlaneAboveSurface();
    glm::vec4 clipPlaneRefraction = prismaState.waterMeshRenderer.GetClippingPlaneBelowSurface();
    if(prismaState.camera.position.y < prismaState.waterMeshRenderer.GetWaterLevel()){
        std::swap(clipPlaneReflection, clipPlaneRefraction);
    }

    // render whole scene (exluding water) from a reflected view            
    DEBUG_GLCHECK( glEnable(GL_CLIP_DISTANCE0); );
    prismaState.camera.Store();
    prismaState.camera.MirrorAtXZPlane(prismaState.waterMeshRenderer.GetWaterLevel());
    prismaState.camera.UpdateUniformBufferObject();
    gBuffer.Use();
    gBuffer.ClearFramebuffer(prismaState.camera.clipFar);
    gBuffer.DrawScene(prismaState, clipPlaneReflection);
    if(prismaConfiguration.engine.enableShadowMapping){
        shadowMapper.Use();
        shadowMapper.DrawScene(prismaState);
    }
    if(prismaConfiguration.engine.enableAmbientOcclusion){
        ambientOcclusion.Use();
        ambientOcclusion.SetPositionTexture(gBuffer.GetPositionTexture());
        ambientOcclusion.SetNormalTexture(gBuffer.GetNormalTexture());
        ambientOcclusion.Draw(prismaState.engineParameter);
    }
    environmentReflection.Use(prismaState.engineParameter);
    environmentReflection.SetShadowCascadeFarPlanes(shadowMapper.GetCascadeFarPlanes());
    environmentReflection.SetPositionTexture(gBuffer.GetPositionTexture());
    environmentReflection.SetNormalTexture(gBuffer.GetNormalTexture());
    environmentReflection.SetDiffuseTexture(gBuffer.GetDiffuseTexture());
    environmentReflection.SetEmissionTexture(gBuffer.GetEmissionReflectionTexture());
    environmentReflection.SetSpecularShininessTexture(gBuffer.GetSpecularShininessTexture());
    environmentReflection.SetFaceNormalTexture(gBuffer.GetFaceNormalTexture());
    environmentReflection.SetOcclusionTexture(ambientOcclusion.GetOcclusionTexture());
    environmentReflection.SetShadowMapTexture(shadowMapper.GetDepthTextureArray());
    environmentReflection.ApplyScene(prismaState);
    environmentReflection.Draw();

    // render whole scene (excluding water) from the default view
    prismaState.camera.Restore();
    prismaState.camera.UpdateUniformBufferObject();
    gBuffer.Use();
    gBuffer.ClearFramebuffer(prismaState.camera.clipFar);
    gBuffer.DrawScene(prismaState, clipPlaneRefraction);
    if(prismaConfiguration.engine.enableShadowMapping){
        shadowMapper.Use();
        shadowMapper.DrawScene(prismaState);
    }
    if(prismaConfiguration.engine.enableAmbientOcclusion){
        ambientOcclusion.Use();
        ambientOcclusion.SetPositionTexture(gBuffer.GetPositionTexture());
        ambientOcclusion.SetNormalTexture(gBuffer.GetNormalTexture());
        ambientOcclusion.Draw(prismaState.engineParameter);
    }
    environmentRefraction.Use(prismaState.engineParameter);
    environmentRefraction.SetShadowCascadeFarPlanes(shadowMapper.GetCascadeFarPlanes());
    environmentRefraction.SetPositionTexture(gBuffer.GetPositionTexture());
    environmentRefraction.SetNormalTexture(gBuffer.GetNormalTexture());
    environmentRefraction.SetDiffuseTexture(gBuffer.GetDiffuseTexture());
    environmentRefraction.SetEmissionTexture(gBuffer.GetEmissionReflectionTexture());
    environmentRefraction.SetSpecularShininessTexture(gBuffer.GetSpecularShininessTexture());
    environmentRefraction.SetFaceNormalTexture(gBuffer.GetFaceNormalTexture());
    environmentRefraction.SetOcclusionTexture(ambientOcclusion.GetOcclusionTexture());
    environmentRefraction.SetShadowMapTexture(shadowMapper.GetDepthTextureArray());
    environmentRefraction.ApplyScene(prismaState);
    environmentRefraction.Draw();
    DEBUG_GLCHECK( glDisable(GL_CLIP_DISTANCE0); );
}

