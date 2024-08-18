#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>

// include all types of messages
#include <AmbientLightMessage.hpp>
#include <AtmosphereMessage.hpp>
#include <CameraMessage.hpp>
#include <ClearMessage.hpp>
#include <DirectionalLightMessage.hpp>
#include <EngineParameterMessage.hpp>
#include <MeshMessage.hpp>
#include <PointLightMessage.hpp>
#include <SpotLightMessage.hpp>
#include <WaterMeshMessage.hpp>


/**
 * @brief Manages all received messages.
 */
class MessageManager {
    public:
        /**
         * @brief Insert received messages to the message queue.
         * @param[inout] messages The new messages to be inserted into the queue. These messages are moved to the queue. This container is empty after this operation.
         */
        void InsertMessages(std::vector<MessageBaseType*>& messages);

        /**
         * @brief Fetch all messages that are currently present in the queue.
         * @return The whole message queue.
         * @details The internal queue is cleared after this operation.
         * @note Make sure to delete the message!
         */
        std::vector<MessageBaseType*> FetchMessages(void);

    private:
        std::vector<MessageBaseType*> messageQueue;   // The internal thread-safe message queue.
        std::mutex mtxMessageQueue;                   // A mutex to protect @ref messageQueue.
};


extern MessageManager messageManager;   // The global message manager.

