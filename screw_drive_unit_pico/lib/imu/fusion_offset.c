/**
 * @file fusion_offset.c
 * @author Seb Madgwick
 * @brief Gyroscope offset correction algorithm for run-time calibration of the
 * gyroscope offset.
 */

//------------------------------------------------------------------------------
// Includes

#include "fusion_offset.h"
#include <math.h> // fabsf

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Cutoff frequency in Hz.
 */
#define CUTOFF_FREQUENCY (0.02f)

/**
 * @brief Timeout in seconds.
 */
#define TIMEOUT (5)

/**
 * @brief Threshold in degrees per second.
 */
#define THRESHOLD (3.0f)

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the gyroscope offset algorithm.
 * @param offset Gyroscope offset algorithm structure.
 * @param sampleRate Sample rate in Hz.
 */
void fusion_offset_init(fusion_offset_t *const offset, const uint16_t sampleRate)
{
    offset->filterCoefficient = 2.0f * (float)M_PI * CUTOFF_FREQUENCY * (1.0f / (float)sampleRate);
    offset->timeout = TIMEOUT * sampleRate;
    offset->timer = 0;
    offset->gyroscopeOffset = FUSION_VECTOR_ZERO;
}

/**
 * @brief Updates the gyroscope offset algorithm and returns the corrected
 * gyroscope measurement.
 * @param offset Gyroscope offset algorithm structure.
 * @param gyroscope Gyroscope measurement in degrees per second.
 * @return Corrected gyroscope measurement in degrees per second.
 */
FusionVector fusion_offset_update(fusion_offset_t *const offset, FusionVector gyroscope)
{

    // Subtract offset from gyroscope measurement
    gyroscope = fusion_vector_subtract(gyroscope, offset->gyroscopeOffset);

    // // Reset timer if gyroscope not stationary
    // if ((fabsf(gyroscope.axis.x) > THRESHOLD) || (fabsf(gyroscope.axis.y) > THRESHOLD) ||
    //     (fabsf(gyroscope.axis.z) > THRESHOLD))
    // {
    //     offset->timer = 0;
    //     return gyroscope;
    // }

    // // Increment timer while gyroscope stationary
    // if (offset->timer < offset->timeout)
    // {
    //     offset->timer++;
    //     return gyroscope;
    // }

    // // Adjust offset if timer has elapsed
    // offset->gyroscopeOffset =
    //     FusionVectorAdd(offset->gyroscopeOffset, FusionVectorMultiplyScalar(gyroscope, offset->filterCoefficient));
    return gyroscope;
}

//------------------------------------------------------------------------------
// End of file
