#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <Camera.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the camera message.
 */
#pragma pack(push, 1)
union CameraMessageUnion {
    struct CameraMessageStruct {
        uint8_t messageType;              // Message type, must be @ref MESSAGE_TYPE_CAMERA.
        uint8_t mode;                     // The camera mode.
        GLfloat position[3];              // The position in world space coordinates.
        GLfloat quaternion[4];            // The unit quaternion representing the orientation of the camera (stored as x,y,z,w).
        GLfloat clipNear;                 // The near clipping plane.
        GLfloat clipFar;                  // The far clipping plane.
        GLfloat orthographicLeft;         // The left border limit for orthographic projection.
        GLfloat orthographicRight;        // The right border limit for orthographic projection.
        GLfloat orthographicBottom;       // The bottom border limit for orthographic projection.
        GLfloat orthographicTop;          // The top border limit for orthographic projection.
        GLfloat perspectiveFieldOfView;   // The field of view angle in radians for perspective projection.
    } protocol;
    uint8_t bytes[sizeof(CameraMessageUnion::CameraMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the camera message.
 */
class CameraMessage: public MessageBaseType {
    public:
        CameraProperties camera;   // The properties of the camera.

        /**
         * @brief Construct a new camera message.
         */
        CameraMessage(): MessageBaseType(MESSAGE_TYPE_CAMERA){}

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(CameraMessageUnion))){
                return 0;
            }
            const CameraMessageUnion* msg = reinterpret_cast<const CameraMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            camera.mode = static_cast<CameraMode>(msg->protocol.mode);
            camera.position.x             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[0]);
            camera.position.y             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[1]);
            camera.position.z             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[2]);
            camera.quaternion.w           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[0]);
            camera.quaternion.x           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[1]);
            camera.quaternion.y           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[2]);
            camera.quaternion.z           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.quaternion[3]);
            camera.clipNear               = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.clipNear);
            camera.clipFar                = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.clipFar);
            camera.orthographicLeft       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.orthographicLeft);
            camera.orthographicRight      = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.orthographicRight);
            camera.orthographicBottom     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.orthographicBottom);
            camera.orthographicTop        = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.orthographicTop);
            camera.perspectiveFieldOfView = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.perspectiveFieldOfView);
            return static_cast<int32_t>(sizeof(CameraMessageUnion));
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = (CAMERA_MODE_PERSPECTIVE == camera.mode) || (CAMERA_MODE_ORTHOGRAPHIC == camera.mode);
            valid &= std::isfinite(camera.position.x);
            valid &= std::isfinite(camera.position.y);
            valid &= std::isfinite(camera.position.z);
            valid &= std::isfinite(camera.quaternion.w);
            valid &= std::isfinite(camera.quaternion.x);
            valid &= std::isfinite(camera.quaternion.y);
            valid &= std::isfinite(camera.quaternion.z);
            valid &= std::isfinite(camera.clipNear);
            valid &= std::isfinite(camera.clipFar);
            valid &= std::isfinite(camera.orthographicLeft);
            valid &= std::isfinite(camera.orthographicRight);
            valid &= std::isfinite(camera.orthographicBottom);
            valid &= std::isfinite(camera.orthographicTop);
            valid &= std::isfinite(camera.perspectiveFieldOfView);
            return valid;
        }
};

