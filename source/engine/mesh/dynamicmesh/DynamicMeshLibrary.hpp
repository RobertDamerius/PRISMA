#pragma once


#include <Common.hpp>
#include <DynamicMeshID.hpp>
#include <DynamicMesh.hpp>
#include <ShaderDynamicMesh.hpp>
#include <DynamicMeshMessage.hpp>


class DynamicMeshLibrary {
    public:
        /**
         * @brief Generate the dynamic mesh library by loading the shader.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            bool success = shaderDynamicMesh.Generate();
            if(!success){
                Delete();
            }
            return success;
        }

        /**
         * @brief Delete the mesh library, remove all mesh objects from the library and delete the GL content for all meshes.
         */
        void Delete(void){
            Clear();
            shaderDynamicMesh.Delete();
        }

        /**
         * @brief Clear the mesh library.
         * @note This function must be called from within the GL main thread.
         */
        void Clear(void){
            for(auto& it : dynamicMeshObjects){
                it.second->Delete();
                delete it.second;
            }
            dynamicMeshObjects.clear();
        }

        /**
         * @brief Process a dynamic mesh message to update an existing mesh or loading a new one.
         * @param[inout] message The dynamic mesh message to be processed.
         * @note This function must be called from within the GL main thread.
         */
        void ProcessDynamicMeshMessage(DynamicMeshMessage& message){
            if(message.shouldBeDeleted){ // delete mesh object
                DeleteDynamicMesh(message.meshID);
            }
            else{ // update or add mesh object
                auto it = dynamicMeshObjects.find(message.meshID);
                if(it != dynamicMeshObjects.end()){
                    UpdateDynamicMesh(it->second, message);
                }
                else{
                    AddDynamicMesh(message);
                }
            }
        }

        /**
         * @brief Draw all dynamic meshes.
         * @param[in] cameraPosition The world-space position of the camera to be used to sort the meshes for rendering.
         * @param[in] projectionView Projection-view matrix to be used to sort the meshes for rendering.
         * @param[in] waterClipPlane The water clipping plane vector (a,b,c,d), where (a,b,c) is the normal vector and d is the distance of the plane from the origin, e.g. a*x + b*y + c*z + d = 0.
         */
        void DrawMeshObjects(glm::vec3 cameraPosition, glm::mat4 projectionView, glm::vec4 waterClipPlane){
            auto meshesToDraw = SortMeshesForRendering(cameraPosition, projectionView);
            if(!meshesToDraw.empty()){
                shaderDynamicMesh.Use();
                shaderDynamicMesh.SetWaterClipPlane(waterClipPlane);
                for(auto&& m : meshesToDraw){
                    shaderDynamicMesh.SetModelMatrix(m->properties.GetModelMatrix());
                    shaderDynamicMesh.SetDiffuseColorMultiplier(m->properties.diffuseColorMultiplier);
                    shaderDynamicMesh.SetSpecularColorMultiplier(m->properties.specularColorMultiplier);
                    shaderDynamicMesh.SetEmissionColorMultiplier(m->properties.emissionColorMultiplier);
                    shaderDynamicMesh.SetShininessMultiplier(m->properties.shininessMultiplier);
                    m->Draw();
                }
            }
        }

        /**
         * @brief Draw the mesh shadows for all dynamic meshes.
         * @param[in] cameraPosition The world-space position of the camera to be used to sort the meshes for rendering.
         * @param[in] projectionView Projection-view matrix to be used to sort the meshes for rendering.
         * @param[in] shaderMeshShadow The mesh shadow shader to be used.
         */
        void DrawMeshShadows(glm::vec3 cameraPosition, glm::mat4 projectionView, const ShaderMeshShadowDepth& shaderMeshShadow){
            const bool shadowCasting = true;
            auto meshesToDraw = SortMeshesForRendering(cameraPosition, projectionView, shadowCasting);
            if(!meshesToDraw.empty()){
                shaderMeshShadow.Use();
                for(auto&& m : meshesToDraw){
                    shaderMeshShadow.SetModelMatrix(m->properties.GetModelMatrix());
                    m->Draw();
                }
            }
        }

    protected:
        ShaderDynamicMesh shaderDynamicMesh;                                  // The shader for the dynamic mesh.
        std::unordered_map<DynamicMeshID, DynamicMesh*> dynamicMeshObjects;   // List of all dynamic mesh objects.

        /**
         * @brief Delete a dynamic mesh object.
         * @param[in] meshID The unique dynamic mesh identifier of the dynamic mesh object to be deleted.
         */
        void DeleteDynamicMesh(const DynamicMeshID meshID){
            auto itObject = dynamicMeshObjects.find(meshID);
            if(itObject != dynamicMeshObjects.end()){
                itObject->second->Delete();
                delete itObject->second;
                dynamicMeshObjects.erase(itObject);
            }
        }

        /**
         * @brief Update an existing dynamic mesh.
         * @param[in] existingMesh The existing dynamic mesh to be updated.
         * @param[in] message The message that contains the object data to be assigned for the existing dynamic mesh.
         */
        void UpdateDynamicMesh(DynamicMesh* existingMesh, const DynamicMeshMessage& message){
            existingMesh->properties = message.properties;
            if(message.updateMesh){
                existingMesh->UpdateMeshData(message.meshVertices, message.meshIndices, message.aabbOfVertices);
            }
        }

        /**
         * @brief Add a dynamic mesh object.
         * @param[in] message The message that contains the new dynamic mesh object to be added.
         */
        void AddDynamicMesh(const DynamicMeshMessage& message){
            DynamicMesh* newMesh = new DynamicMesh();
            newMesh->Generate();
            UpdateDynamicMesh(newMesh, message);
            dynamicMeshObjects.insert({message.meshID, newMesh});
        }

        /**
         * @brief Sort the dynamic meshes for rendering and obtain all meshes that are to be rendered.
         * @param[in] cameraPosition The world-space position of the camera to be used to sort the meshes for rendering.
         * @param[in] cameraProjectionView The projection-view matrix of the camera to be used to check for visible meshes.
         * @param[in] shadowCasting True if sort meshes for shadow casting. If this value is false, then the visible-flag of a mesh is checked, otherwise the castShadow-flag is checked to sort out meshes.
         * @return Vector of pointers to the dynamic meshes to be rendered.
         */
        std::vector<DynamicMesh*> SortMeshesForRendering(glm::vec3 cameraPosition, glm::mat4 cameraProjectionView, bool shadowCasting = false){
            std::vector<DynamicMesh*> result;

            // get all visible meshes and their distance to the camera
            std::vector<std::tuple<double,DynamicMesh*>> visibleMeshes;
            CuboidFrustumCuller culler(cameraProjectionView);
            for(auto& itObject : dynamicMeshObjects){
                // ignore invisible meshes (either for normal rendering or for shadow casting)
                DynamicMesh* obj = itObject.second;
                if(shadowCasting){
                    if(!obj->properties.castShadow){
                        continue;
                    }
                }
                else{
                    if(!obj->properties.visible){
                        continue;
                    }
                }

                // ignore meshes without data
                if(obj->MeshDataIsEmpty()){
                    continue;
                }

                // ignore meshes that are not visible by the camera
                AABB cuboid = obj->GetAABBOfTransformedMesh();
                if(!culler.IsVisible(cuboid)){
                    continue;
                }

                // calculate squared distance and insert to temporary container
                double squaredDistance = CalculateSquaredMeshDistance(obj->properties.position, cameraPosition);
                visibleMeshes.push_back({squaredDistance, obj});
            }

            // sort visible meshes by their distance to the camera
            std::sort(visibleMeshes.begin(), visibleMeshes.end(), [](std::tuple<double,DynamicMesh*> a, std::tuple<double,DynamicMesh*> b){ return std::get<0>(a) < std::get<0>(b); });

            // insert to output
            for(auto&& visibleMesh : visibleMeshes){
                result.push_back(std::get<DynamicMesh*>(visibleMesh));
            }
            return result;
        }

        /**
         * @brief Calculate the squared distance from the camera to the mesh.
         * @param[in] meshPosition The position of the mesh.
         * @param[in] cameraPosition The camera from which to calculate the distance to the mesh.
         * @return Squared distance from the camera position to the mesh position.
         */
        double CalculateSquaredMeshDistance(glm::vec3 meshPosition, glm::vec3 cameraPosition){
            double dx = static_cast<double>(meshPosition.x) - static_cast<double>(cameraPosition.x);
            double dy = static_cast<double>(meshPosition.y) - static_cast<double>(cameraPosition.y);
            double dz = static_cast<double>(meshPosition.z) - static_cast<double>(cameraPosition.z);
            return dx*dx + dy*dy + dz*dz;
        }
};

