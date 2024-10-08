#pragma once


#include <Common.hpp>
#include <NonCopyable.hpp>


/**
 * @brief The event class can be used to wait within a thread for an event that is notified by another thread.
 */
class Event: private NonCopyable {
    public:
        /**
         * @brief Create a new event object.
         */
        Event() noexcept :_flag(-1){}

        /**
         * @brief Notify one thread waiting for this event.
         * @param[in] flag User-specific value that should be forwarded to the waiting thread. Note that -1 is used as default value and therefore to indicate timeout when calling @ref WaitFor.
         */
        void NotifyOne(int flag) noexcept {
            std::unique_lock<std::mutex> lock(mtx);
            notified = true;
            _flag = flag;
            cv.notify_one();
            std::this_thread::yield();
        }

        /**
         * @brief Wait for a notification event. A thread calling this function waits until @ref NotifyOne is called.
         * @return The user-specific value that has been set during the @ref NotifyOne call.
         */
        int Wait(void) noexcept {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this](){ return this->notified; });
            notified = false;
            int ret = _flag;
            _flag = -1;
            return ret;
        }

        /**
         * @brief Wait for a notification event or for a timeout. A thread calling this function waits until @ref NotifyOne is called or the wait timed out.
         * @param[in] timeoutMs Timeout in milliseconds.
         * @return The user-specific value that has been set during the @ref NotifyOne call or -1 in case of timeout.
         */
        int WaitFor(uint32_t timeoutMs) noexcept {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this](){ return this->notified; });
            notified = false;
            int ret = _flag;
            _flag = -1;
            return ret;
        }

        /**
         * @brief Clear a notified event.
         */
        void Clear(void) noexcept {
            std::unique_lock<std::mutex> lock(mtx);
            notified = false;
            _flag = -1;
        }

    private:
        int _flag;                    // User-specific value set during notification. The default value is -1.
        std::mutex mtx;               // The mutex used for event notification.
        std::condition_variable cv;   // The condition variable used for event notification.
        bool notified;                // A boolean flag to prevent spurious wakeups.
};

