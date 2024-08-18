#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <LightID.hpp>
#include <DirectionalLight.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the directional light message.
 */
#pragma pack(push, 1)
union DirectionalLightMessageUnion {
    struct DirectionalLightMessageStruct {
        uint8_t messageType;           // Message type, must be @ref MESSAGE_TYPE_DIRECTIONAL_LIGHT.
        uint32_t directionalLightID;   // A directional light identifier.
        uint8_t shouldBeDeleted:1;     // bit0: True if the directional light should be deleted.
        uint8_t visible:1;             // bit1: True if the directional light should be visible, false otherwise.
        uint8_t unused:6;              // bit2-bit7: unused.
        GLfloat color[3];              // Color of the directional light.
        GLfloat direction[3];          // Normalized direction vector of the directional light.
    } protocol;
    uint8_t bytes[sizeof(DirectionalLightMessageUnion::DirectionalLightMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the directional light message.
 */
class DirectionalLightMessage: public MessageBaseType {
    public:
        DirectionalLightID directionalLightID;   // A directional light identifier.
        bool shouldBeDeleted;                    // True if the directional light should be deleted.
        DirectionalLight directionalLight;       // The directional light object.

        /**
         * @brief Construct a new directional light message.
         */
        DirectionalLightMessage(): MessageBaseType(MESSAGE_TYPE_DIRECTIONAL_LIGHT){
            directionalLightID = 0;
            shouldBeDeleted = false;
        }

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(DirectionalLightMessageUnion))){
                return 0;
            }
            const DirectionalLightMessageUnion* msg = reinterpret_cast<const DirectionalLightMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            directionalLightID           = static_cast<DirectionalLightID>(NetworkUtils::NetworkToNativeByteOrder(msg->protocol.directionalLightID));
            shouldBeDeleted              = static_cast<bool>(msg->protocol.shouldBeDeleted);
            directionalLight.visible     = static_cast<bool>(msg->protocol.visible);
            directionalLight.color.r     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[0]);
            directionalLight.color.g     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[1]);
            directionalLight.color.b     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[2]);
            directionalLight.direction.x = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.direction[0]);
            directionalLight.direction.y = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.direction[1]);
            directionalLight.direction.z = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.direction[2]);
            return static_cast<int32_t>(sizeof(DirectionalLightMessageUnion));
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = true;
            valid &= std::isfinite(directionalLight.color.r);
            valid &= std::isfinite(directionalLight.color.g);
            valid &= std::isfinite(directionalLight.color.b);
            valid &= std::isfinite(directionalLight.direction.x);
            valid &= std::isfinite(directionalLight.direction.y);
            valid &= std::isfinite(directionalLight.direction.z);
            return valid;
        }
};

