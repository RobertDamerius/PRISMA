#pragma once


/**
 * @brief Inherit from this class to make it non-copyable.
 */
class NonCopyable {
    protected:
        constexpr NonCopyable() = default;
        ~NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
};

