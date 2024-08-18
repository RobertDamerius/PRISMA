#pragma once


#include <Common.hpp>
#include <EventContainer.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the window event message.
 */
#pragma pack(push, 1)
union WindowEventMessageUnion {
    struct WindowEventMessageStruct {
        char header[6];                         // Header, must be "PRISMA".
        uint32_t framebufferWidth;              // Current width of the window framebuffer in pixels.
        uint32_t framebufferHeight;             // Current height of the window framebuffer in pixels.
        uint8_t prismaCameraControlEnabled:1;   // bit1: True if the camera is controlled via PRISMA.
        uint8_t unused:7;                       // bit2-bit7: unused.
        uint8_t numberOfKeyEvents;              // The number of key events.
        uint8_t numberOfMouseButtonEvents;      // The number of mouse button events.
        uint8_t numberOfCursorEvents;           // The number of cursor events.
        uint8_t numberOfScrollEvents;           // The number of scroll events.
        // variable-length data goes here
    } protocol;
    uint8_t bytes[sizeof(WindowEventMessageUnion::WindowEventMessageStruct)];
};
union WindowEventDataI3Union {
    struct WindowEventDataI3Struct {
        int32_t keyOrButton;
        int32_t action;
        int32_t mods;
    } values;
    uint8_t bytes[sizeof(WindowEventDataI3Union::WindowEventDataI3Struct)];
};
union WindowEventDataD2Union{
    struct WindowEventDataD2Struct {
        double x;
        double y;
    } values;
    uint8_t bytes[sizeof(WindowEventDataD2Union::WindowEventDataD2Struct)];
};
#pragma pack(pop)


/**
 * @brief Represents the window event message to be send from PRISMA.
 */
class WindowEventMessage {
    public:
        uint32_t framebufferWidth;         // The current framebuffer width of the window in pixels.
        uint32_t framebufferHeight;        // The current framebuffer height of the window in pixels.
        bool prismaCameraControlEnabled;   // True if the camera is controlled via PRISMA.
        EventContainer eventContainer;     // Stores GLFW events.

        /**
         * @brief Construct a new window event message.
         */
        WindowEventMessage(): framebufferWidth(0), framebufferHeight(0), prismaCameraControlEnabled(false){}

        /**
         * @brief Serialize the message.
         * @return Bytes representing the binary message.
         */
        std::vector<uint8_t> Serialize(void){
            eventContainer.LimitNumberOfEvents(255);
            WindowEventMessageUnion msg;
            msg.protocol.header[0]                  = 'P';
            msg.protocol.header[1]                  = 'R';
            msg.protocol.header[2]                  = 'I';
            msg.protocol.header[3]                  = 'S';
            msg.protocol.header[4]                  = 'M';
            msg.protocol.header[5]                  = 'A';
            msg.protocol.framebufferWidth           = NetworkUtils::NetworkToNativeByteOrder(framebufferWidth);
            msg.protocol.framebufferHeight          = NetworkUtils::NetworkToNativeByteOrder(framebufferHeight);
            msg.protocol.prismaCameraControlEnabled = prismaCameraControlEnabled;
            msg.protocol.unused                     = 0;
            msg.protocol.numberOfKeyEvents          = static_cast<uint8_t>(eventContainer.keyEvents.size());
            msg.protocol.numberOfMouseButtonEvents  = static_cast<uint8_t>(eventContainer.mouseButtonEvents.size());
            msg.protocol.numberOfCursorEvents       = static_cast<uint8_t>(eventContainer.cursorEvents.size());
            msg.protocol.numberOfScrollEvents       = static_cast<uint8_t>(eventContainer.scrollEvents.size());
            std::vector<uint8_t> bytes;
            bytes.resize(sizeof(WindowEventMessageUnion) + (eventContainer.keyEvents.size() + eventContainer.mouseButtonEvents.size()) * sizeof(WindowEventDataI3Union) + (eventContainer.cursorEvents.size() + eventContainer.scrollEvents.size()) * sizeof(WindowEventDataD2Union));
            std::memcpy(&bytes[0], &msg.bytes[0], sizeof(msg));
            size_t offset = sizeof(msg);
            for(auto&& e : eventContainer.keyEvents){
                WindowEventDataI3Union msgI3;
                msgI3.values.keyOrButton = NetworkUtils::NetworkToNativeByteOrder(e.key);
                msgI3.values.action      = NetworkUtils::NetworkToNativeByteOrder(e.action);
                msgI3.values.mods        = NetworkUtils::NetworkToNativeByteOrder(e.mods);
                std::memcpy(&bytes[offset], &msgI3.bytes[0], sizeof(msgI3));
                offset += sizeof(msgI3);
            }
            for(auto&& e : eventContainer.mouseButtonEvents){
                WindowEventDataI3Union msgI3;
                msgI3.values.keyOrButton = NetworkUtils::NetworkToNativeByteOrder(e.button);
                msgI3.values.action      = NetworkUtils::NetworkToNativeByteOrder(e.action);
                msgI3.values.mods        = NetworkUtils::NetworkToNativeByteOrder(e.mods);
                std::memcpy(&bytes[offset], &msgI3.bytes[0], sizeof(msgI3));
                offset += sizeof(msgI3);
            }
            for(auto&& e : eventContainer.cursorEvents){
                WindowEventDataD2Union msgD2;
                msgD2.values.x = NetworkUtils::NetworkToNativeByteOrder(e.x);
                msgD2.values.y = NetworkUtils::NetworkToNativeByteOrder(e.y);
                std::memcpy(&bytes[offset], &msgD2.bytes[0], sizeof(msgD2));
                offset += sizeof(msgD2);
            }
            for(auto&& e : eventContainer.scrollEvents){
                WindowEventDataD2Union msgD2;
                msgD2.values.x = NetworkUtils::NetworkToNativeByteOrder(e.x);
                msgD2.values.y = NetworkUtils::NetworkToNativeByteOrder(e.y);
                std::memcpy(&bytes[offset], &msgD2.bytes[0], sizeof(msgD2));
                offset += sizeof(msgD2);
            }
            return bytes;
        }
};

