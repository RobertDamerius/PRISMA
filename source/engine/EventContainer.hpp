#pragma once


#include <Common.hpp>


/**
 * @brief Represents the event data of a key event according to GLFW.
 */
class KeyEventData {
    public:
        int32_t key;      // The keyboard key that was pressed or released.
        int32_t action;   // GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions.
        int32_t mods;     // Bit field describing which modifier keys were held down.

        /**
         * @brief Construct a new key event data object.
         */
        KeyEventData(): key(0), action(0), mods(0){}

        /**
         * @brief Construct a new key event data object.
         * @param[in] key The keyboard key that was pressed or released.
         * @param[in] action One of GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions.
         * @param[in] mods Bit field describing which modifier keys were held down.
         */
        KeyEventData(int key, int action, int mods){
            this->key = static_cast<int32_t>(key);
            this->action = static_cast<int32_t>(action);
            this->mods = static_cast<int32_t>(mods);
        }
};


/**
 * @brief Represents the event data of a mouse button event according to GLFW.
 */
class MouseButtonEventData {
    public:
        int32_t button;   // The mouse button that was pressed or released.
        int32_t action;   // One of GLFW_PRESS or GLFW_RELEASE. Future releases may add more actions.
        int32_t mods;     // Bit field describing which modifier keys were held down.

        /**
         * @brief Construct a new mouse button event data object.
         */
        MouseButtonEventData(): button(0), action(0), mods(0){}

        /**
         * @brief Construct a new mouse button event data object.
         * @param[in] button The mouse button that was pressed or released.
         * @param[in] action One of GLFW_PRESS or GLFW_RELEASE. Future releases may add more actions.
         * @param[in] mods Bit field describing which modifier keys were held down.
         */
        MouseButtonEventData(int button, int action, int mods){
            this->button = static_cast<int32_t>(button);
            this->action = static_cast<int32_t>(action);
            this->mods = static_cast<int32_t>(mods);
        }
};


/**
 * @brief Represents the event data of a cursor event according to GLFW.
 */
class CursorEventData {
    public:
        double x;   // The new cursor x-coordinate, relative to the left edge of the content area.
        double y;   // The new cursor y-coordinate, relative to the top edge of the content area.

        /**
         * @brief Construct a new cursor event data object.
         */
        CursorEventData(): x(0.0), y(0.0){}

        /**
         * @brief Construct a new cursor event data object.
         * @param[in] x The new cursor x-coordinate, relative to the left edge of the content area.
         * @param[in] y The new cursor y-coordinate, relative to the top edge of the content area.
         */
        CursorEventData(double x, double y): x(x), y(y) {}
};


/**
 * @brief Represents the event data of a scroll event according to GLFW.
 */
class ScrollEventData {
    public:
        double x;   // The scroll offset along the x-axis.
        double y;   // The scroll offset along the y-axis.

        /**
         * @brief Construct a new scroll event data object.
         */
        ScrollEventData(): x(0.0), y(0.0){}

        /**
         * @brief Construct a new scroll event data object.
         * @param[in] x The scroll offset along the x-axis.
         * @param[in] y The scroll offset along the y-axis.
         */
        ScrollEventData(double x, double y): x(x), y(y) {}
};


/**
 * @brief Represents a container that holds all required input events.
 */
class EventContainer {
    public:
        std::vector<KeyEventData> keyEvents;                   // All key events.
        std::vector<MouseButtonEventData> mouseButtonEvents;   // All mouse button events.
        std::vector<CursorEventData> cursorEvents;             // All cursor events.
        std::vector<ScrollEventData> scrollEvents;             // All scroll events.

        /**
         * @brief Swap the data of this container with a given event container.
         * @param[in] eventContainer The container with which to swap the event data.
         */
        void Swap(EventContainer& eventContainer){
            keyEvents.swap(eventContainer.keyEvents);
            mouseButtonEvents.swap(eventContainer.mouseButtonEvents);
            cursorEvents.swap(eventContainer.cursorEvents);
            scrollEvents.swap(eventContainer.scrollEvents);
        }

        /**
         * @brief Limit the number of events to an upper boundary.
         * @param[in] N The maximum number of events to be stored in each of the event sub containers.
         */
        void LimitNumberOfEvents(size_t N){
            if(keyEvents.size() > N){
                keyEvents.resize(N);
            }
            if(mouseButtonEvents.size() > N){
                mouseButtonEvents.resize(N);
            }
            if(cursorEvents.size() > N){
                cursorEvents.resize(N);
            }
            if(scrollEvents.size() > N){
                scrollEvents.resize(N);
            }
        }
};

