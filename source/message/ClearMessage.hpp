#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>


/**
 * @brief Protocol definition for the clear message.
 */
#pragma pack(push, 1)
union ClearMessageUnion {
    struct ClearMessageStruct {
        uint8_t messageType;   // Message type, must be @ref MESSAGE_TYPE_CLEAR.
    } protocol;
    uint8_t bytes[sizeof(ClearMessageUnion::ClearMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the clear message.
 */
class ClearMessage: public MessageBaseType {
    public:
        /**
         * @brief Construct a new clear message.
         */
        ClearMessage(): MessageBaseType(MESSAGE_TYPE_CLEAR){}

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(ClearMessageUnion))){
                return 0;
            }
            const ClearMessageUnion* msg = reinterpret_cast<const ClearMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            return static_cast<int32_t>(sizeof(ClearMessageUnion));
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            return true;
        }
};

