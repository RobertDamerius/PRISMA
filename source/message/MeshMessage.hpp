#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <MeshID.hpp>
#include <MeshObject.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the mesh message.
 */
#pragma pack(push, 1)
union MeshMessageUnion {
    struct MeshMessageStruct {
        uint8_t messageType;                  // Message type, must be @ref MESSAGE_TYPE_MESH.
        uint32_t meshID;                      // A unique mesh identifier.
        uint8_t shouldBeDeleted:1;            // bit0: True if the mesh with the @ref meshID should be deleted.
        uint8_t visible:1;                    // bit1: True if this mesh object is visible, false otherwise.
        uint8_t castShadow:1;                 // bit2: True if this mesh casts a shadow, false otherwise.
        uint8_t unused:5;                     // bit3-bit7: unused.
        GLfloat position[3];                  // Position in OpenGL world space coordinates.
        glm::vec4 quaternion;                 // Unit quaternion that represents the orientation of this mesh.
        GLfloat scale[3];                     // Scaling of this mesh object in body frame coordinates.
        GLfloat diffuseColorMultiplier[3];    // Diffuse color multiplier for this mesh.
        GLfloat specularColorMultiplier[3];   // Specular color multiplier for this mesh.
        GLfloat emissionColorMultiplier[3];   // Emission color multiplier for this mesh.
        GLfloat shininessMultiplier;          // Shininess multiplier for this mesh.
        uint16_t nameStringLength;            // String length of the mesh name.
        // variable length ASCII string
    } protocol;
    uint8_t bytes[sizeof(MeshMessageUnion::MeshMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the mesh message.
 */
class MeshMessage: public MessageBaseType {
    public:
        MeshID meshID;          // A unique mesh identifier.
        bool shouldBeDeleted;   // True if the mesh with the @ref meshID should be deleted.
        MeshObject object;      // The mesh object.

        /**
         * @brief Construct a new mesh sessage.
         */
        MeshMessage(): MessageBaseType(MESSAGE_TYPE_MESH){
            meshID = 0;
            shouldBeDeleted = false;
        }

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(MeshMessageUnion))){
                return 0;
            }
            const MeshMessageUnion* msg = reinterpret_cast<const MeshMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            meshID                           = static_cast<MeshID>(NetworkUtils::NetworkToNativeByteOrder(msg->protocol.meshID));
            shouldBeDeleted                  = static_cast<bool>(msg->protocol.shouldBeDeleted);
            object.visible                   = static_cast<bool>(msg->protocol.visible);
            object.castShadow                = static_cast<bool>(msg->protocol.castShadow);
            object.position.x                = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[0]);
            object.position.y                = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[1]);
            object.position.z                = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[2]);
            object.quaternion.w              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[0]);
            object.quaternion.x              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[1]);
            object.quaternion.y              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[2]);
            object.quaternion.z              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[3]);
            object.scale.x                   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scale[0]);
            object.scale.y                   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scale[1]);
            object.scale.z                   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scale[2]);
            object.diffuseColorMultiplier.r  = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.diffuseColorMultiplier[0]);
            object.diffuseColorMultiplier.g  = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.diffuseColorMultiplier[1]);
            object.diffuseColorMultiplier.b  = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.diffuseColorMultiplier[2]);
            object.specularColorMultiplier.r = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColorMultiplier[0]);
            object.specularColorMultiplier.g = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColorMultiplier[1]);
            object.specularColorMultiplier.b = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColorMultiplier[2]);
            object.emissionColorMultiplier.r = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.emissionColorMultiplier[0]);
            object.emissionColorMultiplier.g = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.emissionColorMultiplier[1]);
            object.emissionColorMultiplier.b = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.emissionColorMultiplier[2]);
            object.shininessMultiplier       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.shininessMultiplier);
            int32_t nameLength = static_cast<int32_t>(NetworkUtils::NetworkToNativeByteOrder(msg->protocol.nameStringLength));
            int32_t result = static_cast<int32_t>(sizeof(MeshMessageUnion));
            if(length < (result + nameLength)){
                return 0;
            }
            object.name.clear();
            for(int32_t i = 0; i < nameLength; ++i){
                object.name.push_back(static_cast<char>(bytes[result++]));
            }
            object.SetTypeByName();
            return result;
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = !object.name.empty();
            valid &= std::isfinite(object.position.x);
            valid &= std::isfinite(object.position.y);
            valid &= std::isfinite(object.position.z);
            valid &= std::isfinite(object.quaternion.w);
            valid &= std::isfinite(object.quaternion.x);
            valid &= std::isfinite(object.quaternion.y);
            valid &= std::isfinite(object.quaternion.z);
            valid &= std::isfinite(object.scale.x);
            valid &= std::isfinite(object.scale.y);
            valid &= std::isfinite(object.scale.z);
            valid &= std::isfinite(object.diffuseColorMultiplier.r);
            valid &= std::isfinite(object.diffuseColorMultiplier.g);
            valid &= std::isfinite(object.diffuseColorMultiplier.b);
            valid &= std::isfinite(object.specularColorMultiplier.r);
            valid &= std::isfinite(object.specularColorMultiplier.g);
            valid &= std::isfinite(object.specularColorMultiplier.b);
            valid &= std::isfinite(object.emissionColorMultiplier.r);
            valid &= std::isfinite(object.emissionColorMultiplier.g);
            valid &= std::isfinite(object.emissionColorMultiplier.b);
            valid &= std::isfinite(object.shininessMultiplier);
            return valid;
        }
};

