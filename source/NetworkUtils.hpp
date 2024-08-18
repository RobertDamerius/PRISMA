#pragma once


#include <Common.hpp>


namespace NetworkUtils {


/**
 * @brief Swap endianess of a value.
 * @tparam T Template data type.
 * @param t The value for which to swap the byte order.
 * @return Input value, where the byte order is swapped.
 */
template <typename T> inline T SwapEndian(T t){
    static_assert(sizeof(uint8_t) == 1);
    union {
        T t;
        uint8_t byte[sizeof(T)];
    } source, destination;
    source.t = t;
    for(size_t k = 0; k < sizeof(T); ++k){
        destination.byte[k] = source.byte[sizeof(T) - k - 1];
    }
    return destination.t;
}


/**
 * @brief Swap the network byte order to the native host byte order.
 * @tparam T Template data type.
 * @param t The value for which to swap the byte order.
 * @return Input value, where the byte order is changed from network to native byte order.
 */
template <typename T> inline T NetworkToNativeByteOrder(T t){
    if constexpr (std::endian::native != std::endian::big){
        return SwapEndian(t);
    }
    return t;
}


} /* namespace: NetworkUtils */

