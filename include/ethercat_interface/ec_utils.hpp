/**
 * @file ec_utils.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef EC_UTILS_HPP_
#define EC_UTILS_HPP_

#include <stdint.h>

inline bool isBitSet(const uint16_t& value_to_check, uint8_t index_of_bit)
{
    return (value_to_check & (1 << index_of_bit));
}

inline void setBitAtIndex(uint16_t& value, const uint8_t& index_of_bit)
{
    uint16_t setMask = 1 << index_of_bit;
    value = value | setMask;
}

inline void resetBitAtIndex(uint16_t& value, const uint8_t& index_of_bit)
{
    uint16_t resetMask = 1 << index_of_bit;
    value = value & ~(resetMask);
}

#endif // EC_UTILS_HPP_