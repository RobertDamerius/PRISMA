#pragma once


#include <Common.hpp>


namespace StringHelper {


/**
 * @brief Check whether a given strings ends with a given pattern.
 * @param[in] s The string to be checked.
 * @param[in] pattern The pattern to be considered.
 * @return True if the input string s ends with the specified pattern or if the pattern is empty, false otherwise.
 */
inline bool EndsWith(const std::string s, const std::string pattern) {
    if(pattern.empty()){
        return true;
    }
    if(s.size() < pattern.size()){
        return false;
    }
    return (0 == s.compare(s.size() - pattern.size(), pattern.size(), pattern));
}


/**
 * @brief Split a string at spaces.
 * @param[in] s The string to split.
 * @return List of strings, each indicating a word of the given input string.
 */
inline std::vector<std::string> SplitString(std::string s){
    std::vector<std::string> words;
    std::stringstream ss(s);
    std::string word;
    while(ss >> word){
        if(!word.empty()){
            words.push_back(word);
        }
    }
    return words;
}


/**
 * @brief Convert a given string to a copy of lower ASCII characters.
 * @param[in] s The string to be converted.
 * @return A string where all upper case ASCII characters are converted to lower case ones.
 */
inline std::string ToLowerASCIICopy(const std::string s){
    std::string lowerCase = s;
    for(auto&& c : lowerCase){
        if((c >= 'A') && (c <= 'Z')){
            c = (c - 'A') + 'a';
        }
    }
    return lowerCase;
}


/**
 * @brief Convert a byte to a hexadecimal string.
 * @param[in] byte The byte to be converted.
 * @return The hexadecimal string, e.g. "0xA2".
 */
inline std::string ByteToHexString(uint8_t byte){
    char buf[8];
    sprintf(buf, "0x%02X", byte);
    return std::string(buf);
}


} /* namespace: StringHelper */

