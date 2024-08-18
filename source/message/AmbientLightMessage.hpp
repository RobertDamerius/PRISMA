#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <AmbientLight.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the ambient light message.
 */
#pragma pack(push, 1)
union AmbientLightMessageUnion {
    struct AmbientLightMessageStruct {
        uint8_t messageType;   // Message type, must be @ref MESSAGE_TYPE_AMBIENT_LIGHT.
        GLfloat color[3];      // Color of the ambient light.
    } protocol;
    uint8_t bytes[sizeof(AmbientLightMessageUnion::AmbientLightMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the ambient light message.
 */
class AmbientLightMessage: public MessageBaseType {
    public:
        AmbientLight ambientLight;   // The ambient light object.

        /**
         * @brief Construct a new ambient light message.
         */
        AmbientLightMessage(): MessageBaseType(MESSAGE_TYPE_AMBIENT_LIGHT){}

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(AmbientLightMessageUnion))){
                return 0;
            }
            const AmbientLightMessageUnion* msg = reinterpret_cast<const AmbientLightMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            ambientLight.color.r = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[0]);
            ambientLight.color.g = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[1]);
            ambientLight.color.b = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[2]);
            return static_cast<int32_t>(sizeof(AmbientLightMessageUnion));
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            return std::isfinite(ambientLight.color.r) && std::isfinite(ambientLight.color.g) && std::isfinite(ambientLight.color.b);
        }
};

