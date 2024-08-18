#pragma once


#include <Common.hpp>
#include <MeshLibraryBase.hpp>
#include <Camera.hpp>
#include <CuboidFrustumCuller.hpp>
#include <ShaderMeshShadowDepth.hpp>
#include <ShaderAlphaMeshShadowDepth.hpp>


/**
 * @brief This class extends the MeshLibraryBase by drawing calls.
 */
class MeshLibrary: public MeshLibraryBase {
    public:
        /**
         * @brief Draw all mesh objects.
         * @param[in] cameraPosition The world-space position of the camera to be used to sort the meshes for rendering.
         * @param[in] projectionView Projection-view matrix to be used to sort the meshes for rendering.
         * @param[in] waterClipPlane The water clipping plane vector (a,b,c,d), where (a,b,c) is the normal vector and d is the distance of the plane from the origin, e.g. a*x + b*y + c*z + d = 0.
         * @details GL content of meshes and textures may be generated if required.
         */
        void DrawMeshObjects(glm::vec3 cameraPosition, glm::mat4 projectionView, glm::vec4 waterClipPlane){
            const std::lock_guard<std::mutex> lock(mtxObjectsAndData);
            auto [colorMeshesToDraw, textureMeshesToDraw, transparentTextureMeshesToDraw] = SortMeshesForRendering(cameraPosition, projectionView);
            DrawColorMeshes(colorMeshesToDraw, waterClipPlane);
            DrawTextureMeshes(textureMeshesToDraw, waterClipPlane);
            DEBUG_GLCHECK( glDisable(GL_CULL_FACE); );
            DrawTextureMeshes(transparentTextureMeshesToDraw, waterClipPlane);
            DEBUG_GLCHECK( glEnable(GL_CULL_FACE); );
            if(informMeshLoader){
                eventMeshLoader.NotifyOne(0);
                informMeshLoader = false;
            }
        }

        /**
         * @brief Draw the mesh shadows for all objects.
         * @param[in] cameraPosition The world-space position of the camera to be used to sort the meshes for rendering.
         * @param[in] projectionView Projection-view matrix to be used to sort the meshes for rendering.
         * @param[in] shaderMeshShadow The mesh shadow shader to be used.
         * @param[in] shaderAlphaMeshShadow The alpha mesh shadow shader to be used.
         * @details GL content of meshes and textures may be generated if required.
         */
        void DrawMeshShadows(glm::vec3 cameraPosition, glm::mat4 projectionView, const ShaderMeshShadowDepth& shaderMeshShadow, const ShaderAlphaMeshShadowDepth& shaderAlphaMeshShadow){
            const std::lock_guard<std::mutex> lock(mtxObjectsAndData);
            const bool shadowCasting = true;
            auto [colorMeshesToDraw, textureMeshesToDraw, transparentTextureMeshesToDraw] = SortMeshesForRendering(cameraPosition, projectionView, shadowCasting);
            if(!colorMeshesToDraw.empty() || !textureMeshesToDraw.empty()){
                shaderMeshShadow.Use();
                DrawColorMeshShadows(colorMeshesToDraw, shaderMeshShadow);
                DrawTextureMeshShadows(textureMeshesToDraw, shaderMeshShadow);
            }
            if(!transparentTextureMeshesToDraw.empty()){
                shaderAlphaMeshShadow.Use();
                DEBUG_GLCHECK( glDisable(GL_CULL_FACE); );
                DrawTextureAlphaMeshShadows(transparentTextureMeshesToDraw, shaderAlphaMeshShadow);
                DEBUG_GLCHECK( glEnable(GL_CULL_FACE); );
            }
        }

    protected:
        /**
         * @brief Sort the mesh objects for rendering and obtain all color and texture meshes that are to be rendered.
         * @param[in] cameraPosition The world-space position of the camera to be used to sort the meshes for rendering.
         * @param[in] cameraProjectionView The projection-view matrix of the camera to be used to check for visible meshes.
         * @param[in] shadowCasting True if sort meshes for shadow casting. If this value is false, then the visible-flag of a mesh is checked, otherwise the castShadow-flag is checked to sort out meshes.
         * @return A tuple containing the following values:
         * [0] Vector of pairs containing a color mesh object and the corresponding mesh data.
         * [1] Vector of pairs containing a texture mesh object and the corresponding mesh data.
         * [2] Vector of pairs containing a transparent texture mesh object and the corresponding mesh data.
         */
        std::tuple<std::vector<std::pair<MeshObject*,MeshData*>>, std::vector<std::pair<MeshObject*,MeshData*>>, std::vector<std::pair<MeshObject*,MeshData*>>> SortMeshesForRendering(glm::vec3 cameraPosition, glm::mat4 cameraProjectionView, bool shadowCasting = false){
            std::tuple<std::vector<std::pair<MeshObject*,MeshData*>>, std::vector<std::pair<MeshObject*,MeshData*>>, std::vector<std::pair<MeshObject*,MeshData*>>> result;
            std::vector<std::pair<MeshObject*,MeshData*>>& colorMeshes = std::get<0>(result);
            std::vector<std::pair<MeshObject*,MeshData*>>& textureMeshes = std::get<1>(result);
            std::vector<std::pair<MeshObject*,MeshData*>>& transparentTextureMeshes = std::get<2>(result);

            // get all visible meshes and their distance to the camera
            std::vector<std::tuple<double,MeshObject*,MeshData*>> visibleMeshes;
            CuboidFrustumCuller culler(cameraProjectionView);
            for(auto& itObject : meshObjects){
                // ignore invisible meshes (either for normal rendering or for shadow casting)
                MeshObject* obj = itObject.second;
                if(shadowCasting){
                    if(!obj->castShadow){
                        continue;
                    }
                }
                else{
                    if(!obj->visible){
                        continue;
                    }
                }

                // ignore meshes without data
                auto itData = meshData.find(obj->name);
                if(itData == meshData.end()){
                    continue;
                }
                MeshData* data = &(itData->second);

                // ignore meshes that are not loaded
                if(!data->loaded){
                    continue;
                }

                // ignore meshes that are not visible by the camera
                AABB cuboid = data->data->GetAABBOfVertices();
                cuboid.Transform(obj->GetModelMatrix());
                if(!culler.IsVisible(cuboid)){
                    continue;
                }

                // calculate squared distance and insert to temporary container
                double squaredDistance = CalculateSquaredMeshDistance(obj->position, cameraPosition);
                visibleMeshes.push_back({squaredDistance, obj, data});
            }

            // sort visible meshes by their distance to the camera
            std::sort(visibleMeshes.begin(), visibleMeshes.end(), [](std::tuple<double,MeshObject*,MeshData*> a, std::tuple<double,MeshObject*,MeshData*> b){ return std::get<0>(a) < std::get<0>(b); });

            // insert to output
            for(auto&& visibleMesh : visibleMeshes){
                const MeshType& type = std::get<MeshObject*>(visibleMesh)->type;
                if(MESH_TYPE_COLORMESH == type){
                    colorMeshes.push_back({std::get<MeshObject*>(visibleMesh), std::get<MeshData*>(visibleMesh)});
                }
                else if(MESH_TYPE_TEXTUREMESH == type){
                    bool isTransparent = reinterpret_cast<TextureMesh*>(std::get<MeshData*>(visibleMesh)->data)->IsTransparent();
                    if(isTransparent){
                        transparentTextureMeshes.push_back({std::get<MeshObject*>(visibleMesh), std::get<MeshData*>(visibleMesh)});
                    }
                    else{
                        textureMeshes.push_back({std::get<MeshObject*>(visibleMesh), std::get<MeshData*>(visibleMesh)});
                    }
                }
            }
            return result;
        }

        /**
         * @brief Draw color mesh objects.
         * @param[in] meshesToDraw Vector of all meshes to be rendered.
         * @param[in] waterClipPlane The water clipping plane vector (a,b,c,d), where (a,b,c) is the normal vector and d is the distance of the plane from the origin, e.g. a*x + b*y + c*z + d = 0.
         */
        void DrawColorMeshes(std::vector<std::pair<MeshObject*,MeshData*>> meshesToDraw, glm::vec4 waterClipPlane){
            if(!meshesToDraw.empty()){
                shaderColorMesh.Use();
                shaderColorMesh.SetWaterClipPlane(waterClipPlane);
                for(auto&& m : meshesToDraw){
                    ColorMesh* internalMeshData = reinterpret_cast<ColorMesh*>(m.second->data);

                    // generate GL content if not generated
                    if(m.second->loaded && !m.second->generated){
                        (void) internalMeshData->Generate();
                        m.second->generated = true;
                    }

                    // set uniforms and draw mesh
                    if(m.second->generated){
                        shaderColorMesh.SetModelMatrix(m.first->GetModelMatrix());
                        shaderColorMesh.SetDiffuseColorMultiplier(m.first->diffuseColorMultiplier);
                        shaderColorMesh.SetSpecularColorMultiplier(m.first->specularColorMultiplier);
                        shaderColorMesh.SetEmissionColorMultiplier(m.first->emissionColorMultiplier);
                        shaderColorMesh.SetShininessMultiplier(m.first->shininessMultiplier);
                        internalMeshData->Draw();
                    }
                }
            }
        }

        /**
         * @brief Draw texture mesh objects.
         * @param[in] meshesToDraw Vector of all meshes to be rendered.
         * @param[in] waterClipPlane The water clipping plane vector (a,b,c,d), where (a,b,c) is the normal vector and d is the distance of the plane from the origin, e.g. a*x + b*y + c*z + d = 0.
         */
        void DrawTextureMeshes(std::vector<std::pair<MeshObject*,MeshData*>> meshesToDraw, glm::vec4 waterClipPlane){
            if(!meshesToDraw.empty()){
                shaderTextureMesh.Use();
                shaderTextureMesh.SetWaterClipPlane(waterClipPlane);
                for(auto&& m : meshesToDraw){
                    TextureMesh* internalMeshData = reinterpret_cast<TextureMesh*>(m.second->data);

                    // generate GL content if not generated
                    if(m.second->loaded && !m.second->generated){
                        (void) internalMeshData->Generate();
                        m.second->generated = true;
                    }

                    // set uniforms and draw mesh
                    if(m.second->generated){
                        shaderTextureMesh.SetModelMatrix(m.first->GetModelMatrix());
                        internalMeshData->Draw(shaderTextureMesh, m.first->diffuseColorMultiplier, m.first->specularColorMultiplier, m.first->emissionColorMultiplier, m.first->shininessMultiplier);
                    }
                }
            }
        }

        /**
         * @brief Draw depth of color mesh objects for shadow mapping.
         * @param[in] meshesToDraw Vector of all meshes to be rendered.
         * @param[in] shader The mesh shadow depth shader in use.
         */
        void DrawColorMeshShadows(std::vector<std::pair<MeshObject*,MeshData*>> meshesToDraw, const ShaderMeshShadowDepth& shader){
            for(auto&& m : meshesToDraw){
                ColorMesh* internalMeshData = reinterpret_cast<ColorMesh*>(m.second->data);

                // generate GL content if not generated
                if(m.second->loaded && !m.second->generated){
                    (void) internalMeshData->Generate();
                    m.second->generated = true;
                }

                // set uniforms and draw mesh
                if(m.second->generated){
                    shader.SetModelMatrix(m.first->GetModelMatrix());
                    internalMeshData->Draw();
                }
            }
        }

        /**
         * @brief Draw depth of texture mesh objects for shadow mapping.
         * @param[in] meshesToDraw Vector of all meshes to be rendered.
         * @param[in] shader The mesh shadow depth shader in use.
         */
        void DrawTextureMeshShadows(std::vector<std::pair<MeshObject*,MeshData*>> meshesToDraw, const ShaderMeshShadowDepth& shader){
            for(auto&& m : meshesToDraw){
                TextureMesh* internalMeshData = reinterpret_cast<TextureMesh*>(m.second->data);

                // generate GL content if not generated
                if(m.second->loaded && !m.second->generated){
                    (void) internalMeshData->Generate();
                    m.second->generated = true;
                }

                // set uniforms and draw mesh
                if(m.second->generated){
                    shader.SetModelMatrix(m.first->GetModelMatrix());
                    internalMeshData->DrawWithoutMaterial();
                }
            }
        }

        /**
         * @brief Draw depth of texture alpha mesh objects for shadow mapping.
         * @param[in] meshesToDraw Vector of all meshes to be rendered.
         * @param[in] shader The mesh shadow depth shader in use.
         */
        void DrawTextureAlphaMeshShadows(std::vector<std::pair<MeshObject*,MeshData*>> meshesToDraw, const ShaderAlphaMeshShadowDepth& shader){
            for(auto&& m : meshesToDraw){
                TextureMesh* internalMeshData = reinterpret_cast<TextureMesh*>(m.second->data);

                // generate GL content if not generated
                if(m.second->loaded && !m.second->generated){
                    (void) internalMeshData->Generate();
                    m.second->generated = true;
                }

                // set uniforms and draw mesh
                if(m.second->generated){
                    shader.SetModelMatrix(m.first->GetModelMatrix());
                    internalMeshData->DrawWithoutMaterialButDiffuseMap();
                }
            }
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

