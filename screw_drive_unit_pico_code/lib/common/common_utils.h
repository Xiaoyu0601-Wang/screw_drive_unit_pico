#ifndef _COMMON_UTILS_H_
#define _COMMON_UTILS_H_

typedef union
{
    int16_t data;  // 16-bit value
    
    struct
    {
        uint8_t lsb;  // Lower 8 bits
        uint8_t msb; // Higher 8 bits
    } element;
} data16_t;

typedef union
{
    float f;
    int32_t i;
} union32_t;

#endif
