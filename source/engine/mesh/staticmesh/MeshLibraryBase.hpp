#pragma once


#include <Common.hpp>
#include <Event.hpp>
#include <FileManager.hpp>
#include <StringHelper.hpp>
#include <ShaderColorMesh.hpp>
#include <ShaderTextureMesh.hpp>
#include <MeshID.hpp>
#include <MeshObject.hpp>
#include <MeshMessage.hpp>
#include <MeshBase.hpp>
#include <ColorMesh.hpp>
#include <TextureMesh.hpp>


/**
 * @brief This base class represents the mesh library that contains all mesh objects that are present in the scene and the
 * mesh data such as VBOs, textures, and so on. If mesh data has not been loaded, then it is loaded in a separate thread.
 */
class MeshLibraryBase {
    public:
        /**
         * @brief Generate the mesh library by loading shaders and starting the loader thread.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            bool success = shaderColorMesh.Generate();
            success &= shaderTextureMesh.Generate();
            if(success){
                StartMeshLoaderThread();
            }
            else{
                Delete();
            }
            return success;
        }

        /**
         * @brief Delete the mesh library, remove all mesh objects from the library and delete the GL content for all meshes.
         */
        void Delete(void){
            StopMeshLoaderThread();
            Clear();
            shaderColorMesh.Delete();
            shaderTextureMesh.Delete();
        }

        /**
         * @brief Start the protection to protect the library against changes from the internal loading thread. End the protection via @ref EndProtection.
         * @details The library has to be protected when using @ref Clear and @ref ProcessMeshMessage.
         */
        void StartProtection(void){ mtxObjectsAndData.lock(); }

        /**
         * @brief End the protection that has been started via @ref StartProtection.
         * @details The library has to be protected when using @ref Clear and @ref ProcessMeshMessage.
         */
        void EndProtection(void){ mtxObjectsAndData.unlock(); }

        /**
         * @brief Clear the mesh library.
         * @note This function must be called from within the GL main thread. Make sure that the library is protected via @ref StartProtection / @ref EndProtection when calling this function.
         * Otherwise the internal loading thread may cause race conditions and undefined behaviour.
         */
        void Clear(void){
            // delete objects
            for(auto& it : meshObjects){
                delete it.second;
            }
            meshObjects.clear();

            // delete data
            for(auto& it : meshData){
                it.second.data->Delete();
                delete it.second.data;
            }
            meshData.clear();
        }

        /**
         * @brief Process a mesh message to update an existing mesh or loading a new one.
         * @param[inout] message The mesh message to be processed. The type of the mesh is set by its name.
         * @details If the mesh data has not been read, a separate thread loads the required file data and a successive @ref Draw call is going to generate the GL content.
         * If the shouldBeDeleted flag is set, then all other object data is ignored and the mesh object with the specified ID is deleted.
         * @note This function must be called from within the GL main thread. Make sure that the library is protected via @ref StartProtection / @ref EndProtection when calling this function.
         * Otherwise the internal loading thread may cause race conditions and undefined behaviour.
         */
        void ProcessMeshMessage(MeshMessage& message){
            if(message.shouldBeDeleted){ // delete mesh object
                DeleteMeshObject(message.meshID);
            }
            else{
                // ignore invalid mesh type
                message.object.SetTypeByName();
                if(MESH_TYPE_INVALID == message.object.type){
                    return;
                }

                // update or add mesh object
                auto it = meshObjects.find(message.meshID);
                if(it != meshObjects.end()){
                    UpdateMeshObject(it->second, message);
                }
                else{
                    AddMeshObject(message);
                }
            }
        }

    protected:
        ShaderColorMesh shaderColorMesh;                   // The shader to render color meshes.
        ShaderTextureMesh shaderTextureMesh;               // The shader to render texture meshes.

        /* All mesh objects are stored in the @ref meshObjects container. They are indicated by a unique mesh ID. */
        /* The actual data such as VBOs, textures, etc. is stored in separate data containers. */
        std::unordered_map<MeshID, MeshObject*> meshObjects;   // List of all mesh objects.
        struct MeshData {                                      // Represents the value of a mesh data entry.
            MeshBase* data;                                    // Mesh data.
            bool loaded;                                       // True if mesh data has been loaded from a file.
            bool generated;                                    // True if GL content has been generated.
            int64_t objectCounter;                             // Number of objects that refer to this data by their mesh names.
        };
        std::unordered_map<std::string, MeshData> meshData;    // Container of all mesh data.
        std::mutex mtxObjectsAndData;                          // Protect @ref objects and all data containers.

        /**
         * @brief Delete a mesh object.
         * @param[in] meshID The unique mesh identifier of the mesh object to be deleted.
         */
        void DeleteMeshObject(const MeshID meshID){
            auto itObject = meshObjects.find(meshID);
            if(itObject != meshObjects.end()){
                // check if mesh data should be deleted
                std::string meshName = itObject->second->name;
                auto itData = meshData.find(meshName);
                if(itData != meshData.end()){
                    itData->second.objectCounter--;
                    if(itData->second.objectCounter < 1){ // no more objects that uses this mesh data: delete it
                        itData->second.data->Delete();
                        delete itData->second.data;
                        meshData.erase(itData);
                    }
                }

                // delete object
                delete itObject->second;
                meshObjects.erase(itObject);
            }
        }

        /**
         * @brief Update an existing mesh.
         * @param[in] existingMesh The existing mesh to be updated.
         * @param[in] message The message that contains the object data to be assigned for the existing mesh.
         * @details The object data of the message is only assigned, if the mesh type and the name are correct.
         */
        void UpdateMeshObject(MeshObject* existingMesh, const MeshMessage& message){
            if((existingMesh->type == message.object.type) && (0 == message.object.name.compare(existingMesh->name))){
                *existingMesh = message.object;
            }
        }

        /**
         * @brief Add a mesh object.
         * @param[in] message The message that contains the new mesh object to be added.
         * @details If the mesh data required for the new mesh does not exist, it is added to the mesh loader thread.
         */
        void AddMeshObject(const MeshMessage& message){
            MeshObject* newMesh = new MeshObject();
            *newMesh = message.object;
            meshObjects.insert({message.meshID, newMesh});

            // add mesh data if it does not exist or increment object counter if it exists
            auto itData = meshData.find(newMesh->name);
            if(itData == meshData.end()){
                MeshData emptyData;
                if(newMesh->type == MESH_TYPE_COLORMESH){
                    emptyData.data = new ColorMesh();
                }
                else{
                    emptyData.data = new TextureMesh();
                }
                emptyData.loaded = false;
                emptyData.generated = false;
                emptyData.objectCounter = 1;
                meshData.insert({newMesh->name, emptyData});
                AddToMeshLoader(newMesh->name);
            }
            else{
                itData->second.objectCounter++;
            }
        }

        /**
         * @brief Add a new mesh name to the mesh loader.
         * @param[in] meshName The mesh name to be loaded.
         */
        void AddToMeshLoader(std::string meshName){
            const std::lock_guard<std::mutex> lock(mtxMeshNamesToLoad);
            meshNamesToLoad.push_back(meshName);
            informMeshLoader = true;
        }


        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Mesh Loader Thread Helper Functions
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        /* attributes related to the mesh loader thread */
        std::thread threadMeshLoader;               // The internal thread object.
        std::atomic<bool> terminateThread;          // Thread termination flag.
        Event eventMeshLoader;                      // Event to notify the loader thread.
        std::vector<std::string> meshNamesToLoad;   // List of mesh messages to be loaded by the loader thread.
        std::mutex mtxMeshNamesToLoad;              // Protect the @ref messagesToLoad container.
        bool informMeshLoader;                      // Flag that indicates if something has been added to the mesh loader.

        /**
         * @brief Start the mesh loader thread.
         */
        void StartMeshLoaderThread(void){
            threadMeshLoader = std::thread(&MeshLibraryBase::MeshLoaderThread, this);
        }

        /**
         * @brief Stop the mesh loader thread.
         */
        void StopMeshLoaderThread(void){
            terminateThread = true;
            eventMeshLoader.NotifyOne(0);
            if(threadMeshLoader.joinable()){
                threadMeshLoader.join();
            }
            eventMeshLoader.Clear();
            terminateThread = false;
            mtxMeshNamesToLoad.lock();
            meshNamesToLoad.clear();
            mtxMeshNamesToLoad.unlock();
        }

        /**
         * @brief The mesh loader thread function.
         */
        void MeshLoaderThread(void){
            while(!terminateThread){
                // wait for something to be loaded
                eventMeshLoader.Wait();
                if(terminateThread){
                    break;
                }

                // get mesh names to be loaded
                std::vector<std::string> meshNames;
                mtxMeshNamesToLoad.lock();
                meshNames.swap(meshNamesToLoad);
                mtxMeshNamesToLoad.unlock();

                // make meshNames unique
                std::sort(meshNames.begin(), meshNames.end());
                meshNames.erase(std::unique(meshNames.begin(), meshNames.end()), meshNames.end());

                // load all meshes
                std::vector<std::future<void>> asyncResults;
                for(auto&& meshName : meshNames){
                    try{ asyncResults.push_back(std::async(std::launch::async, &MeshLibraryBase::LoadMeshFromFile, this, meshName)); } catch(...){}
                }
                for(auto&& ar : asyncResults){
                    try{ ar.get(); } catch(...){}
                }
            }
        }

        /**
         * @brief Load a mesh from a file.
         * @param[in] meshName The mesh name that defines the mesh to be loaded.
         */
        void LoadMeshFromFile(std::string meshName){
            MeshBase* meshData = nullptr;
            if(StringHelper::EndsWith(meshName, ".ply")){
                meshData = new ColorMesh();
            }
            else if(StringHelper::EndsWith(meshName, ".obj")){
                meshData = new TextureMesh();
            }
            if(meshData){
                if(!LoadMeshData(meshName, meshData)){
                    delete meshData;
                }
                else if(!AssignMeshData(meshName, meshData)){
                    delete meshData;
                }
            }
        }

        /**
         * @brief Load a mesh from a file.
         * @param[in] meshName The name of the mesh.
         * @param[in] mesh The mesh to be read.
         * @return True if success, false otherwise.
         */
        bool LoadMeshData(std::string meshName, MeshBase* mesh) const {
            std::string filename = MeshNameToFilename(meshName);
            return mesh->ReadFromFile(filename);
        }

        /**
         * @brief Convert a mesh name to the corresponding filename of the mesh.
         * @param[in] meshName The mesh name to be converted to the filename.
         * @return The filename to the mesh file.
         */
        std::string MeshNameToFilename(std::string meshName) const {
            std::filesystem::path filename = FileName(FILENAME_DIRECTORY_MESH);
            filename /= meshName;
            return filename.string();
        }

        /**
         * @brief Assign a new mesh data to the library.
         * @param[in] meshName The name of the mesh for which to assign new data.
         * @param[in] newMeshData The new mesh data to be set.
         * @return True if newMeshData has been set, false otherwise.
         */
        bool AssignMeshData(std::string meshName, MeshBase* newMeshData){
            const std::lock_guard<std::mutex> lock(mtxObjectsAndData);
            auto it = meshData.find(meshName);
            if(it == meshData.end()){ // entry does not exist, unable to assign
                return false;
            }
            if(it->second.loaded || it->second.generated){ // data has already been loaded or generated
                return false;
            }

            // delete current data entry and replace by newMeshData
            delete it->second.data;
            it->second.data = newMeshData;
            it->second.loaded = true;
            return true;
        }
};

