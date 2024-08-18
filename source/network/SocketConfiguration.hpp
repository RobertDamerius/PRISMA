#pragma once


#include <Common.hpp>


/**
 * @brief Represents the configuration for a UDP multicast socket.
 */
class SocketConfiguration {
    public:
        int32_t socketPriority;                              // The socket priority (linux only, in range [0 (lowest), 6 (greatest)]).
        uint16_t localPort;                                  // The local port to be bound.
        uint8_t ttl;                                         // The time-to-live (TTL) value to be set for multicast messages.
        std::vector<std::array<uint8_t,4>> groupAddresses;   // The group addresses to be joined.
        std::array<uint8_t,4> txInterfaceAddress;            // IPv4 address of the network interface to be used for sending multicast messages.
        std::string txInterfaceName;                         // Name of the network interface to be used for sending the multicast messages.
        bool useTxInterfaceName;                             // True if @ref txInterfaceName should be used instead of @ref txInterfaceAddress.
        std::array<uint8_t,4> joinInterfaceAddress;          // IPv4 address of the network interface to be used for joining the multicast group.
        std::string joinInterfaceName;                       // Name of the network interface to be used for joining the multicast group.
        bool useJoinInterfaceName;                           // True if @ref joinInterfaceName should be used instead of @ref joinInterfaceAddress.

        /**
         * @brief Construct a new network configuration object and set default values.
         */
        SocketConfiguration(){ Clear(); }

        /**
         * @brief Clear configuration and set default values.
         */
        void Clear(void){
            socketPriority = 0;
            localPort = 0;
            ttl = 0;
            groupAddresses = {{239,0,0,0}};
            txInterfaceAddress = {0,0,0,0};
            txInterfaceName = "";
            useTxInterfaceName = false;
            joinInterfaceAddress = {0,0,0,0};
            joinInterfaceName = "";
            useJoinInterfaceName = false;
        }
};

