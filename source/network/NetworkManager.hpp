#pragma once


#include <Common.hpp>
#include <MulticastUDPSocket.hpp>
#include <MessageManager.hpp>
#include <Event.hpp>
#include <PrismaConfiguration.hpp>
#include <StringHelper.hpp>


/**
 * @brief The network manager handles the UDP multicast socket and receives and processes messages.
 * Successfully received messages are inserted to the @ref MessageManager.
 */
class NetworkManager {
    public:
        /**
         * @brief Start the network manager by setting destination addresses and launching a separate thread.
         */
        void Start(void){
            Stop();
            for(auto&& address : prismaConfiguration.network.send.destinationAddresses){
                IPAddress dst;
                dst.ip[0] = static_cast<uint8_t>(address[0]);
                dst.ip[1] = static_cast<uint8_t>(address[1]);
                dst.ip[2] = static_cast<uint8_t>(address[2]);
                dst.ip[3] = static_cast<uint8_t>(address[3]);
                dst.port = address[4];
                destinations.push_back(dst);
            }
            networkMainThread = std::thread(&NetworkManager::NetworkMainThread, this);
        }

        /**
         * @brief Stop the network manager.
         */
        void Stop(void){
            terminate = true;
            udpSocket.Close();
            udpRetryTimer.NotifyOne(0);
            if(networkMainThread.joinable()){
                networkMainThread.join();
            }
            udpRetryTimer.Clear();
            terminate = false;
            destinations.clear();
        }

        /**
         * @brief Send a window event message via the UDP socket.
         * @param[in] msg The message to be send.
         * @details If the socket is not open, then the message is discarded.
         */
        void SendWindowEventMessage(WindowEventMessage& msg){
            for(auto&& destination : destinations){
                std::vector<uint8_t> bytes = msg.Serialize();
                (void) udpSocket.SendTo(destination, &bytes[0], static_cast<int32_t>(bytes.size()));
            }
        }

    private:
        std::thread networkMainThread;         // Thread object for the internal network manager thread.
        std::atomic<bool> terminate;           // True if the thread should be terminated, false otherwise.
        MulticastUDPSocket udpSocket;          // The multicast UDP socket.
        Event udpRetryTimer;                   // A timer to wait before retrying to initialize a UDP socket in case of errors.
        std::vector<IPAddress> destinations;   // The destination addresses to which to send messages. The destinations are set during @ref Start.

        /**
         * @brief The main thread function of the network manager.
         */
        void NetworkMainThread(void){
            // local buffer where to store received messages
            constexpr size_t rxBufferSize = 65507;
            uint8_t* rxBuffer = new uint8_t[rxBufferSize];

            // get network configuration from PRISMA configuration file
            SocketConfiguration conf;
            conf.localPort            = prismaConfiguration.network.localPort;
            conf.groupAddresses       = prismaConfiguration.network.receive.multicastGroups;
            conf.joinInterfaceAddress = prismaConfiguration.network.interfaceAddress;
            conf.joinInterfaceName    = prismaConfiguration.network.interfaceName;
            conf.useJoinInterfaceName = !prismaConfiguration.network.interfaceName.empty();
            conf.socketPriority       = prismaConfiguration.network.socketPriority;
            conf.ttl                  = prismaConfiguration.network.send.timeToLive;
            conf.txInterfaceAddress   = prismaConfiguration.network.interfaceAddress;
            conf.txInterfaceName      = prismaConfiguration.network.interfaceName;
            conf.useTxInterfaceName   = !prismaConfiguration.network.interfaceName.empty();

            // main loop of the network manager
            while(!terminate){
                // (re)-initialize the socket operation
                if(!udpSocket.Open(conf)){
                    udpRetryTimer.WaitFor(prismaConfiguration.network.socketErrorRetryTimeMs);
                    continue;
                }

                // receive messages and process them
                IPAddress source;
                while(!terminate && udpSocket.IsOpen()){
                    udpSocket.ResetLastError();
                    int32_t rx = udpSocket.ReceiveFrom(source, &rxBuffer[0], rxBufferSize);
                    auto [errorCode, errorStringIgnored] = udpSocket.GetLastError();
                    if(!udpSocket.IsOpen() || terminate){
                        break;
                    }
                    if(rx < 0){
                        #ifdef _WIN32
                        if(WSAEMSGSIZE == errorCode){
                            continue;
                        }
                        #endif
                        udpRetryTimer.WaitFor(prismaConfiguration.network.socketErrorRetryTimeMs);
                        break;
                    }
                    std::string errorString = ProcessReceivedMessage(&rxBuffer[0], rx);
                    if(!errorString.empty()){
                        udpSocket.SendTo(source, reinterpret_cast<const uint8_t*>(errorString.c_str()), static_cast<int32_t>(errorString.length()));
                    }
                    std::this_thread::yield();
                }

                // terminate the socket
                udpSocket.Close();
            }
            delete[] rxBuffer;
        }

        /**
         * @brief Process a received UDP message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The length of the received UDP message.
         * @return An empty string if success or an error message if processing failed.
         */
        std::string ProcessReceivedMessage(const uint8_t* bytes, int32_t length){
            std::vector<MessageBaseType*> newMessages;
            int32_t i = 0;
            int32_t remainingBytes = length;
            while(i < length){
                MessageType messageType = static_cast<MessageType>(bytes[i]);
                switch(messageType){
                    case MESSAGE_TYPE_CLEAR:               newMessages.push_back(new ClearMessage());              break;
                    case MESSAGE_TYPE_ENGINE_PARAMETER:    newMessages.push_back(new EngineParameterMessage());    break;
                    case MESSAGE_TYPE_CAMERA:              newMessages.push_back(new CameraMessage());             break;
                    case MESSAGE_TYPE_ATMOSPHERE:          newMessages.push_back(new AtmosphereMessage());         break;
                    case MESSAGE_TYPE_AMBIENT_LIGHT:       newMessages.push_back(new AmbientLightMessage());       break;
                    case MESSAGE_TYPE_DIRECTIONAL_LIGHT:   newMessages.push_back(new DirectionalLightMessage());   break;
                    case MESSAGE_TYPE_POINT_LIGHT:         newMessages.push_back(new PointLightMessage());         break;
                    case MESSAGE_TYPE_SPOT_LIGHT:          newMessages.push_back(new SpotLightMessage());          break;
                    case MESSAGE_TYPE_MESH:                newMessages.push_back(new MeshMessage());               break;
                    case MESSAGE_TYPE_WATER_MESH:          newMessages.push_back(new WaterMeshMessage());          break;
                    case MESSAGE_TYPE_DYNAMIC_MESH:        newMessages.push_back(new DynamicMeshMessage());        break;
                    default:
                        for(auto&& m : newMessages){
                            delete m;
                        }
                        return "unknown message type " + StringHelper::ByteToHexString(static_cast<uint8_t>(messageType));
                }
                int32_t numDeserializedBytes = newMessages.back()->Deserialize(&bytes[i], remainingBytes);
                if(numDeserializedBytes < 1){
                    for(auto&& m : newMessages){
                        delete m;
                    }
                    return "invalid format of message " + StringHelper::ByteToHexString(static_cast<uint8_t>(messageType));
                }
                if(!newMessages.back()->IsValid()){
                    for(auto&& m : newMessages){
                        delete m;
                    }
                    return "invalid data of message " + StringHelper::ByteToHexString(static_cast<uint8_t>(messageType));
                }
                i += numDeserializedBytes;
                remainingBytes -= numDeserializedBytes;
            }
            messageManager.InsertMessages(newMessages);
            return "";
        }
};

