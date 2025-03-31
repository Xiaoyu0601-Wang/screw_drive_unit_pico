#ifndef _FUSION_OFFSET_H_
#define _FUSION_OFFSET_H_

//------------------------------------------------------------------------------
// Includes

#include "math_utils.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Gyroscope offset algorithm structure.  Structure members are used
 * internally and must not be accessed by the application.
 */
typedef struct
{
    float filterCoefficient;
    unsigned int timeout;
    unsigned int timer;
    FusionVector gyroscopeOffset;
} fusion_offset_t;

//------------------------------------------------------------------------------
// Function declarations

void fusion_offset_init(fusion_offset_t *const offset, const uint16_t sampleRate);
FusionVector fusion_offset_update(fusion_offset_t *const offset, FusionVector gyroscope);

#endif

//------------------------------------------------------------------------------
// End of file
