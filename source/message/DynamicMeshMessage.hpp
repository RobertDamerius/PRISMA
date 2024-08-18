#pragma once


#include <Common.hpp>
#include <DynamicMeshID.hpp>
#include <MessageBaseType.hpp>
#include <DynamicMeshProperties.hpp>
#include <AABB.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the dynamic mesh message.
 */
#pragma pack(push, 1)
union DynamicMeshMessageUnion {
    struct DynamicMeshMessageStruct {
        uint8_t messageType;                      // Message type, must be @ref MESSAGE_TYPE_DYNAMIC_MESH.
        uint32_t meshID;                          // A unique mesh identifier.
        uint8_t shouldBeDeleted:1;                // bit0: True if the mesh with the @ref meshID should be deleted.
        uint8_t updateMesh:1;                     // bit1: True if the mesh should be updated, false otherwise.
        uint8_t visible:1;                        // bit2: True if the mesh should be visible, false otherwise.
        uint8_t castShadow:1;                     // bit3: True if this mesh casts a shadow, false otherwise.
        uint8_t unused:4;                         // bit4-bit7: unused.
        GLfloat position[3];                      // Position in OpenGL world space coordinates.
        glm::vec4 quaternion;                     // Unit quaternion that represents the orientation of this mesh.
        GLfloat scale[3];                         // Scaling of this mesh object in body frame coordinates.
        GLfloat diffuseColorMultiplier[3];        // Diffuse color multiplier for this mesh.
        GLfloat specularColorMultiplier[3];       // Specular color multiplier for this mesh.
        GLfloat emissionColorMultiplier[3];       // Emission color multiplier for this mesh.
        GLfloat shininessMultiplier;              // Shininess multiplier for this mesh.
        // variable mesh data
    } protocol;
    uint8_t bytes[sizeof(DynamicMeshMessageUnion::DynamicMeshMessageStruct)];
};
union DynamicMeshMessageVertexUnion {
    struct DynamicMeshMessageVertexStruct {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat r;
        GLfloat g;
        GLfloat b;
    } vertex;
    uint8_t bytes[sizeof(DynamicMeshMessageVertexUnion::DynamicMeshMessageVertexStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the dynamic mesh message.
 */
class DynamicMeshMessage: public MessageBaseType {
    public:
        DynamicMeshID meshID;                          // A unique dynamic mesh identifier.
        bool shouldBeDeleted;                          // True if the mesh with the @ref meshID should be deleted.
        bool updateMesh;                               // True if the mesh should be updated, false otherwise.
        DynamicMeshProperties properties;              // The properties of the dynamic mesh.
        std::vector<DynamicMeshVertex> meshVertices;   // Vertices of the dynamic mesh.
        std::vector<GLuint> meshIndices;               // Indices to that vertices representing the triangles of the dynamic mesh.
        AABB aabbOfVertices;                           // The axis-aligned bounding box of @ref meshVertices.

        /**
         * @brief Construct a new dynamic mesh message.
         */
        DynamicMeshMessage(): MessageBaseType(MESSAGE_TYPE_DYNAMIC_MESH){
            updateMesh = true;
        }

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(DynamicMeshMessageUnion))){
                return 0;
            }
            const DynamicMeshMessageUnion* msg = reinterpret_cast<const DynamicMeshMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            meshID                               = static_cast<DynamicMeshID>(NetworkUtils::NetworkToNativeByteOrder(msg->protocol.meshID));
            shouldBeDeleted                      = static_cast<bool>(msg->protocol.shouldBeDeleted);
            updateMesh                           = static_cast<bool>(msg->protocol.updateMesh);
            properties.visible                   = static_cast<bool>(msg->protocol.visible);
            properties.castShadow                = static_cast<bool>(msg->protocol.castShadow);
            properties.position.x                = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[0]);
            properties.position.y                = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[1]);
            properties.position.z                = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[2]);
            properties.quaternion.w              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[0]);
            properties.quaternion.x              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[1]);
            properties.quaternion.y              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[2]);
            properties.quaternion.z              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[3]);
            properties.scale.x                   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scale[0]);
            properties.scale.y                   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scale[1]);
            properties.scale.z                   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scale[2]);
            properties.diffuseColorMultiplier.r  = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.diffuseColorMultiplier[0]);
            properties.diffuseColorMultiplier.g  = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.diffuseColorMultiplier[1]);
            properties.diffuseColorMultiplier.b  = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.diffuseColorMultiplier[2]);
            properties.specularColorMultiplier.r = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColorMultiplier[0]);
            properties.specularColorMultiplier.g = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColorMultiplier[1]);
            properties.specularColorMultiplier.b = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColorMultiplier[2]);
            properties.emissionColorMultiplier.r = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.emissionColorMultiplier[0]);
            properties.emissionColorMultiplier.g = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.emissionColorMultiplier[1]);
            properties.emissionColorMultiplier.b = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.emissionColorMultiplier[2]);
            properties.shininessMultiplier       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.shininessMultiplier);

            // deserialize variable mesh data
            int32_t result = static_cast<int32_t>(sizeof(DynamicMeshMessageUnion));
            if((result + 2) > length){
                return 0;
            }
            int32_t numVertices = static_cast<int32_t>(NetworkUtils::NetworkToNativeByteOrder(*reinterpret_cast<const uint16_t*>(&bytes[result])));
            result += 2;
            if((result + numVertices * sizeof(DynamicMeshMessageVertexUnion)) > static_cast<size_t>(length)){
                return 0;
            }
            meshVertices.clear();
            aabbOfVertices.Clear();
            glm::vec3 minPosition(std::numeric_limits<GLfloat>::infinity());
            glm::vec3 maxPosition(-std::numeric_limits<GLfloat>::infinity());
            for(int32_t v = 0; v < numVertices; ++v){
                const DynamicMeshMessageVertexUnion* vData = reinterpret_cast<const DynamicMeshMessageVertexUnion*>(&bytes[result]);
                meshVertices.push_back(DynamicMeshVertex());
                meshVertices.back().position[0] = NetworkUtils::NetworkToNativeByteOrder(vData->vertex.x);
                meshVertices.back().position[1] = NetworkUtils::NetworkToNativeByteOrder(vData->vertex.y);
                meshVertices.back().position[2] = NetworkUtils::NetworkToNativeByteOrder(vData->vertex.z);
                meshVertices.back().color[0]    = NetworkUtils::NetworkToNativeByteOrder(vData->vertex.r);
                meshVertices.back().color[1]    = NetworkUtils::NetworkToNativeByteOrder(vData->vertex.g);
                meshVertices.back().color[2]    = NetworkUtils::NetworkToNativeByteOrder(vData->vertex.b);
                minPosition.x = std::min(minPosition.x, meshVertices.back().position[0]);
                minPosition.y = std::min(minPosition.y, meshVertices.back().position[1]);
                minPosition.z = std::min(minPosition.z, meshVertices.back().position[2]);
                maxPosition.x = std::max(maxPosition.x, meshVertices.back().position[0]);
                maxPosition.y = std::max(maxPosition.y, meshVertices.back().position[1]);
                maxPosition.z = std::max(maxPosition.z, meshVertices.back().position[2]);
                result += sizeof(DynamicMeshMessageVertexUnion);
            }
            if(std::isfinite(minPosition.x)){
                aabbOfVertices.lowestPosition = minPosition;
                aabbOfVertices.dimension = maxPosition - minPosition;
            }
            if((result + 2) > length){
                return 0;
            }
            int32_t numIndices = static_cast<int32_t>(NetworkUtils::NetworkToNativeByteOrder(*reinterpret_cast<const uint16_t*>(&bytes[result])));
            result += 2;
            if((result + numIndices * sizeof(uint16_t)) > static_cast<size_t>(length)){
                return 0;
            }
            for(int32_t i = 0; i < numIndices; ++i){
                meshIndices.push_back(static_cast<GLuint>(NetworkUtils::NetworkToNativeByteOrder(*reinterpret_cast<const uint16_t*>(&bytes[result]))));
                if(static_cast<int32_t>(meshIndices.back()) >= numVertices){
                    return 0;
                }
                result += sizeof(uint16_t);
            }
            return result;
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = true;
            valid &= std::isfinite(properties.position.x);
            valid &= std::isfinite(properties.position.y);
            valid &= std::isfinite(properties.position.z);
            valid &= std::isfinite(properties.quaternion.x);
            valid &= std::isfinite(properties.quaternion.y);
            valid &= std::isfinite(properties.quaternion.z);
            valid &= std::isfinite(properties.quaternion.w);
            valid &= std::isfinite(properties.scale.x);
            valid &= std::isfinite(properties.scale.y);
            valid &= std::isfinite(properties.scale.z);
            valid &= std::isfinite(properties.diffuseColorMultiplier.x);
            valid &= std::isfinite(properties.diffuseColorMultiplier.y);
            valid &= std::isfinite(properties.diffuseColorMultiplier.z);
            valid &= std::isfinite(properties.specularColorMultiplier.x);
            valid &= std::isfinite(properties.specularColorMultiplier.y);
            valid &= std::isfinite(properties.specularColorMultiplier.z);
            valid &= std::isfinite(properties.emissionColorMultiplier.x);
            valid &= std::isfinite(properties.emissionColorMultiplier.y);
            valid &= std::isfinite(properties.emissionColorMultiplier.z);
            valid &= std::isfinite(properties.shininessMultiplier);
            return valid;
        }
};

