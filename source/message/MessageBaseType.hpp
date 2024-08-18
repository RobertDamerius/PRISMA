#pragma once


#include <Common.hpp>


/**
 * @brief All possible message types.
 */
enum MessageType : uint8_t {
    MESSAGE_TYPE_CLEAR = 0x00,
    MESSAGE_TYPE_ENGINE_PARAMETER = 0x01,
    MESSAGE_TYPE_CAMERA = 0x02,
    MESSAGE_TYPE_ATMOSPHERE = 0x03,
    MESSAGE_TYPE_AMBIENT_LIGHT = 0x04,
    MESSAGE_TYPE_DIRECTIONAL_LIGHT = 0x05,
    MESSAGE_TYPE_POINT_LIGHT = 0x06,
    MESSAGE_TYPE_SPOT_LIGHT = 0x07,
    MESSAGE_TYPE_MESH = 0x08,
    MESSAGE_TYPE_WATER_MESH = 0x09,
    MESSAGE_TYPE_DYNAMIC_MESH = 0x0A
};


/**
 * @brief The base class of a message.
 */
class MessageBaseType {
    public:
        const MessageType messageType;   // The type indicating the message.

        /**
         * @brief Construct a new base type for a message.
         * @param[in] type The type of this message.
         */
        explicit MessageBaseType(MessageType type): messageType(type) {}

        /**
         * @brief Destroy this message.
         */
        virtual ~MessageBaseType(){}

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        virtual int32_t Deserialize(const uint8_t* bytes, int32_t length) = 0;

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        virtual bool IsValid(void) = 0;
};

