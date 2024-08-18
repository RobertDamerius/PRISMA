#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <LightID.hpp>
#include <SpotLight.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the spot light message.
 */
#pragma pack(push, 1)
union SpotLightMessageUnion {
    struct SpotLightMessageStruct {
        uint8_t messageType;           // Message type, must be @ref MESSAGE_TYPE_SPOT_LIGHT.
        uint32_t spotLightID;          // A spot light identifier.
        uint8_t shouldBeDeleted:1;     // bit0: True if the spot light with the @ref spotLightID should be deleted.
        uint8_t visible:1;             // bit1: True if the spot light should be visible, false otherwise.
        uint8_t unused:6;              // bit2-bit7: unused.
        GLfloat color[3];              // Color of the spot light.
        GLfloat position[3];           // World-space position of the spot light.
        GLfloat direction[3];          // Normalized direction vector of the spot light.
        GLfloat quadraticFalloff;      // Coefficient for light attenuation according to 1 / (1 + c*x^2).
        GLfloat invRadiusPow4;         // Inverse of radius to the power of 4, where the radius represents the range of the light.
        GLfloat cosInnerCutOffAngle;   // Cosine of the inner cut-off angle of light cone.
        GLfloat cosOuterCutOffAngle;   // Cosine of the outer cut-off angle of light cone.
    } protocol;
    uint8_t bytes[sizeof(SpotLightMessageUnion::SpotLightMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the spot light message.
 */
class SpotLightMessage: public MessageBaseType {
    public:
        SpotLightID spotLightID;   // A spot light identifier.
        bool shouldBeDeleted;      // True if the spot light with the @ref spotLightID should be deleted.
        SpotLight spotLight;       // The spot light object.

        /**
         * @brief construct a new spot light message.
         */
        SpotLightMessage(): MessageBaseType(MESSAGE_TYPE_SPOT_LIGHT){
            spotLightID = 0;
            shouldBeDeleted = false;
        }

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(SpotLightMessageUnion))){
                return 0;
            }
            const SpotLightMessageUnion* msg = reinterpret_cast<const SpotLightMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            spotLightID                   = static_cast<SpotLightID>(NetworkUtils::NetworkToNativeByteOrder(msg->protocol.spotLightID));
            shouldBeDeleted               = static_cast<bool>(msg->protocol.shouldBeDeleted);
            spotLight.visible             = static_cast<bool>(msg->protocol.visible);
            spotLight.color.r             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[0]);
            spotLight.color.g             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[1]);
            spotLight.color.b             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.color[2]);
            spotLight.position.x          = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[0]);
            spotLight.position.y          = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[1]);
            spotLight.position.z          = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[2]);
            spotLight.direction.x         = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.direction[0]);
            spotLight.direction.y         = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.direction[1]);
            spotLight.direction.z         = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.direction[2]);
            spotLight.quadraticFalloff    = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quadraticFalloff);
            spotLight.invRadiusPow4       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.invRadiusPow4);
            spotLight.cosInnerCutOffAngle = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.cosInnerCutOffAngle);
            spotLight.cosOuterCutOffAngle = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.cosOuterCutOffAngle);
            return static_cast<int32_t>(sizeof(SpotLightMessageUnion));
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = true;
            valid &= std::isfinite(spotLight.color.r);
            valid &= std::isfinite(spotLight.color.g);
            valid &= std::isfinite(spotLight.color.b);
            valid &= std::isfinite(spotLight.position.x);
            valid &= std::isfinite(spotLight.position.y);
            valid &= std::isfinite(spotLight.position.z);
            valid &= std::isfinite(spotLight.direction.x);
            valid &= std::isfinite(spotLight.direction.y);
            valid &= std::isfinite(spotLight.direction.z);
            valid &= std::isfinite(spotLight.quadraticFalloff);
            valid &= std::isfinite(spotLight.invRadiusPow4);
            valid &= std::isfinite(spotLight.cosInnerCutOffAngle);
            valid &= std::isfinite(spotLight.cosOuterCutOffAngle);
            return valid;
        }
};

