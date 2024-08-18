#pragma once


#include <Common.hpp>
#include <Camera.hpp>
#include <MeshLibrary.hpp>
#include <DynamicMeshLibrary.hpp>
#include <LightLibrary.hpp>
#include <Atmosphere.hpp>
#include <WaterMeshRenderer.hpp>
#include <EngineParameter.hpp>
#include <PrismaConfiguration.hpp>


class PrismaState {
    public:
        Camera camera;                             // The view camera of the scene.
        MeshLibrary meshLibrary;                   // The library that contains all objects to be considered for rendering.
        DynamicMeshLibrary dynamicMeshLibrary;     // The library that contains all dynamic meshes to be considered for rendering.
        LightLibrary lightLibrary;                 // The library that contains light light casters to be considered for rendering.
        Atmosphere atmosphere;                     // The atmosphere that contains sunlight, fog, etc.
        WaterMeshRenderer waterMeshRenderer;       // The water mesh renderer.
        EngineParameter engineParameter;           // The runtime parameter for the PRISMA engine.

        /**
         * @brief Clear the whole PRISMA state and set default values.
         * @param[in] clearCamera True if the camera should be cleared, false otherwise.
         */
        void Clear(bool clearCamera){
            if(clearCamera){
                SetInitialCameraValues();
            }
            meshLibrary.Clear();
            dynamicMeshLibrary.Clear();
            lightLibrary.Clear();
            atmosphere.Clear();
            waterMeshRenderer.Clear();
            engineParameter.Clear();
        }

        /**
         * @brief Resize the scene.
         * @param[in] width Width in pixels.
         * @param[in] height Height in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            camera.Resize(width, height);
        }

        /**
         * @brief Generate the PRISMA state and all its GL content.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            camera.GenerateUniformBufferObject(UBO_CAMERA);
            bool success = meshLibrary.Generate();
            success &= dynamicMeshLibrary.Generate();
            success &= waterMeshRenderer.Generate();
            lightLibrary.Generate(SSBO_DIRECTIONALLIGHTS, SSBO_POINTLIGHTS, SSBO_SPOTLIGHTS, prismaConfiguration.engine.maxNumDirectionalLights, prismaConfiguration.engine.maxNumPointLights, prismaConfiguration.engine.maxNumSpotLights);
            atmosphere.Generate(UBO_ATMOSPHERE);
            Clear(true);
            return success;
        }

        /**
         * @brief Delete the PRISMA state and all its GL content.
         */
        void Delete(void){
            waterMeshRenderer.Delete();
            atmosphere.Delete();
            meshLibrary.Delete();
            dynamicMeshLibrary.Delete();
            lightLibrary.Delete();
            camera.DeleteUniformBufferObject();
        }

    private:
        /**
         * @brief Set initial values for the @ref camera.
         */
        void SetInitialCameraValues(void){
            camera.clipNear = 0.1f;
            camera.clipFar = 1000.0f;
            camera.mode = CAMERA_MODE_PERSPECTIVE;
            camera.perspectiveFieldOfView = glm::radians(70.0f);
            camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
            camera.quaternion = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            camera.orthographicLeft = -1.0f;
            camera.orthographicRight = 1.0f;
            camera.orthographicBottom = -1.0f;
            camera.orthographicTop = 1.0f;
            camera.UpdateUniformBufferObject();
        }
};

