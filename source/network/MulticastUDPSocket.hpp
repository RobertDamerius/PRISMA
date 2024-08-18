#pragma once


#include <Common.hpp>
#include <UDPSocket.hpp>
#include <SocketConfiguration.hpp>


class MulticastUDPSocket: protected UDPSocket {
    public:
        using UDPSocket::IsOpen;
        using UDPSocket::ReceiveFrom;
        using UDPSocket::SendTo;
        using UDPSocket::GetLastError;
        using UDPSocket::ResetLastError;

        /**
         * @brief Open the multicast UDP socket.
         * @param[in] configuration The socket configuration to be used to open the multicast UDP socket.
         * @return True if success, false otherwise.
         */
        bool Open(const SocketConfiguration configuration){
            conf = configuration;

            // Open the UDP socket
            ResetLastError();
            if(!UDPSocket::Open()){
                auto [errorCode, errorString] = GetLastError();
                PrintE("Could not open UDP socket! %s\n", errorString.c_str());
                return false;
            }

            // Set priority
            #ifndef _WIN32
            int priority = static_cast<int>(conf.socketPriority);
            ResetLastError();
            if(SetOption(SOL_SOCKET, SO_PRIORITY, &priority, sizeof(priority)) < 0){
                auto [errorCode, errorString] = GetLastError();
                PrintW("Could not set socket priority %d for UDP socket! %s\n", conf.socketPriority, errorString.c_str());
            }
            #endif

            // Reuse port and ignore errors
            ResetLastError();
            if(ReusePort(true) < 0){
                auto [errorCode, errorString] = GetLastError();
                PrintW("Could not set reuse port option for UDP socket! %s\n", errorString.c_str());
            }

            // Bind the port (ALWAYS USE ANY INTERFACE!)
            ResetLastError();
            if(Bind(conf.localPort) < 0){
                auto [errorCode, errorString] = GetLastError();
                PrintE("Could not bind port %d for UDP socket! %s\n", conf.localPort, errorString.c_str());
                UDPSocket::Close();
                return false;
            }

            // Set TTL
            ResetLastError();
            if(SetMulticastTTL(conf.ttl) < 0){
                auto [errorCode, errorString] = GetLastError();
                PrintW("Could not set TTL %u for UDP socket! %s\n", conf.ttl, errorString.c_str());
            }

            // set interface and join group for all desired group addresses
            for(auto&& groupAddress : conf.groupAddresses){
                // set multicast interface for outgoing traffic
                ResetLastError();
                if(SetMulticastInterface(groupAddress, conf.txInterfaceAddress, conf.txInterfaceName, conf.useTxInterfaceName) < 0){
                    auto [errorCode, errorString] = GetLastError();
                    if(conf.useTxInterfaceName){
                        PrintE("Could not set multicast interface \"%s\" for outgoing traffic! %s\n", conf.txInterfaceName.c_str(), errorString.c_str());
                    }
                    else{
                        PrintE("Could not set multicast interface %u.%u.%u.%u for outgoing traffic! %s\n", conf.txInterfaceAddress[0], conf.txInterfaceAddress[1], conf.txInterfaceAddress[2], conf.txInterfaceAddress[3], errorString.c_str());
                    }
                    UDPSocket::Close();
                    return false;
                }

                // join multicast group
                ResetLastError();
                if(JoinMulticastGroup(groupAddress, conf.joinInterfaceAddress, conf.joinInterfaceName, conf.useJoinInterfaceName) < 0){
                    auto [errorCode, errorString] = GetLastError();
                    if(conf.useJoinInterfaceName){
                        PrintE("Could not join multicast group %u.%u.%u.%u at interface \"%s\"! %s\n", groupAddress[0], groupAddress[1], groupAddress[2], groupAddress[3], conf.joinInterfaceName.c_str(), errorString.c_str());
                    }
                    else{
                        PrintE("Could not join multicast group %u.%u.%u.%u at interface %u.%u.%u.%u! %s\n", groupAddress[0], groupAddress[1], groupAddress[2], groupAddress[3], conf.joinInterfaceAddress[0], conf.joinInterfaceAddress[1], conf.joinInterfaceAddress[2], conf.joinInterfaceAddress[3], errorString.c_str());
                    }
                    UDPSocket::Close();
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Close the multicast UDP socket.
         */
        void Close(void){
            for(auto&& groupAddress : conf.groupAddresses){
                LeaveMulticastGroup(groupAddress, conf.joinInterfaceAddress, conf.joinInterfaceName, conf.useJoinInterfaceName);
            }
            UDPSocket::Close();
        }

    private:
        SocketConfiguration conf;   // The socket configuration that has been used to open the socket.
};

