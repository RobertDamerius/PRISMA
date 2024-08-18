#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <LightID.hpp>
#include <PointLight.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the point light message.
 */
#pragma pack(push, 1)
union PointLightMessageUnion {
    struct PointLightMessageStruct {
        uint8_t messageType;         // Message type, must be @ref MESSAGE_TYPE_POINT_LIGHT.
        uint32_t pointLightID;       // A unique point light identifier.
        uint8_t shouldBeDeleted:1;   // bit0: True if the point light with the @ref pointLightID should be deleted.
        uint8_t visible:1;           // bit1: True if the point light should be visible, false otherwise.
        uint8_t unused:6;            // bit2-bit7: unused.
        GLfloat color[3];            // The color of the point light.
        GLfloat position[3];         // The world-space position of the point light.
        GLfloat quadraticFalloff;    // Coefficient for light attenuation according to 1 / (1 + c*x^2).
        GLfloat invRadiusPow4;       // Inverse of radius to the power of 4, where the radius represents the range of the light.
    } protocol;
    uint8_t bytes[sizeof(PointLightMessageUnion::PointLightMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the point light message.
 */
class PointLightMessage: public MessageBaseType {
    public:
        PointLightID pointLightID;   // A unique point light identifier.
        bool shouldBeDeleted;        // True if the point light with the @ref pointLightID should be deleted.
        PointLight pointLight;       // The point light object.

        /**
         * @brief Construct a new point light message.
         */
        PointLightMessage(): MessageBaseType(MESSAGE_TYPE_POINT_LIGHT){
            pointLightID = 0;
            shouldBeDeleted = false;
        }

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(PointLightMessageUnion))){
                return 0;
            }
            const PointLightMessageUnion* msg = reinterpret_cast<const PointLightMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            pointLightID                = static_cast<PointLightID>(NetworkUtils::NetworkToNativeByteOrder(msg->protocol.pointLightID));
            shouldBeDeleted             = static_cast<bool>(msg->protocol.shouldBeDeleted);
            pointLight.visible          = static_cast<bool>(msg->protocol.visible);
            pointLight.color.r          = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[0]);
            pointLight.color.g          = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[1]);
            pointLight.color.b          = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[2]);
            pointLight.position.x       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[0]);
            pointLight.position.y       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[1]);
            pointLight.position.z       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[2]);
            pointLight.quadraticFalloff = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quadraticFalloff);
            pointLight.invRadiusPow4    = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.invRadiusPow4);
            return static_cast<int32_t>(sizeof(PointLightMessageUnion));
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = true;
            valid &= std::isfinite(pointLight.color.r);
            valid &= std::isfinite(pointLight.color.g);
            valid &= std::isfinite(pointLight.color.b);
            valid &= std::isfinite(pointLight.position.x);
            valid &= std::isfinite(pointLight.position.y);
            valid &= std::isfinite(pointLight.position.z);
            valid &= std::isfinite(pointLight.quadraticFalloff);
            valid &= std::isfinite(pointLight.invRadiusPow4);
            return valid;
        }
};

