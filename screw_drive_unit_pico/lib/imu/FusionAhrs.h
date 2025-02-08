/**
 * @file FusionAhrs.h
 * @author Seb Madgwick
 * @brief AHRS algorithm to combine gyroscope, accelerometer, and magnetometer
 * measurements into a single measurement of orientation relative to the Earth.
 */

#ifndef _FUSION_AHRS_H_
#define _FUSION_AHRS_H_

//------------------------------------------------------------------------------
// Includes
#include <stdbool.h>
#include "math_utils.h"
#include "FusionConvention.h"
#include "fusion_offset.h"



//------------------------------------------------------------------------------
// Definitions

/**
 * @brief AHRS algorithm settings.
 */
typedef struct
{
    FusionConvention convention;
    uint16_t sample_rate;
    float sample_period;
    float gain;
    float gyroscopeRange;
    float accelerationRejection;
    float magneticRejection;
    unsigned int recoveryTriggerPeriod;
} FusionAhrsSettings;

/**
 * @brief AHRS algorithm structure.  Structure members are used internally and
 * must not be accessed by the application.
 */
typedef struct
{
    uint16_t sample_rate;
    FusionAhrsSettings settings;
    fusion_offset_t offset;
    FusionQuaternion quaternion;
    FusionVector accelerometer;
    bool initialising;
    float rampedGain;
    float rampedGainStep;
    bool angularRateRecovery;
    FusionVector halfAccelerometerFeedback;
    FusionVector halfMagnetometerFeedback;
    bool accelerometerIgnored;
    int accelerationRecoveryTrigger;
    int accelerationRecoveryTimeout;
    bool magnetometerIgnored;
    int magneticRecoveryTrigger;
    int magneticRecoveryTimeout;
} fusion_ahrs_t;

/**
 * @brief AHRS algorithm internal states.
 */
typedef struct
{
    float accelerationError;
    bool accelerometerIgnored;
    float accelerationRecoveryTrigger;
    float magneticError;
    bool magnetometerIgnored;
    float magneticRecoveryTrigger;
} FusionAhrsInternalStates;

/**
 * @brief AHRS algorithm flags.
 */
typedef struct
{
    bool initialising;
    bool angularRateRecovery;
    bool accelerationRecovery;
    bool magneticRecovery;
} FusionAhrsFlags;

//------------------------------------------------------------------------------
// Function declarations

void fusion_ahrs_init(fusion_ahrs_t *const ahrs, uint16_t sample_rate);

void fusion_ahrs_reset(fusion_ahrs_t *const ahrs);

void fusionAhrs_set_settings(fusion_ahrs_t *const ahrs, const FusionAhrsSettings *const settings);

void fusion_ahrs_update(fusion_ahrs_t *const ahrs, const FusionVector gyroscope, const FusionVector accelerometer,
                        const FusionVector magnetometer, const float deltaTime);

void fusion_ahrs_update_no_magnetometer(fusion_ahrs_t *const ahrs, const FusionVector gyroscope,
                                    const FusionVector accelerometer, const float deltaTime);

void FusionAhrsUpdateExternalHeading(fusion_ahrs_t *const ahrs, const FusionVector gyroscope,
                                     const FusionVector accelerometer, const float heading, const float deltaTime);

FusionQuaternion FusionAhrsGetQuaternion(const fusion_ahrs_t *const ahrs);

void FusionAhrsSetQuaternion(fusion_ahrs_t *const ahrs, const FusionQuaternion quaternion);

FusionVector FusionAhrsGetGravity(const fusion_ahrs_t *const ahrs);

FusionVector FusionAhrsGetLinearAcceleration(const fusion_ahrs_t *const ahrs);

FusionVector FusionAhrsGetEarthAcceleration(const fusion_ahrs_t *const ahrs);

FusionAhrsInternalStates FusionAhrsGetInternalStates(const fusion_ahrs_t *const ahrs);

FusionAhrsFlags FusionAhrsGetFlags(const fusion_ahrs_t *const ahrs);

void FusionAhrsSetHeading(fusion_ahrs_t *const ahrs, const float heading);

#endif

//------------------------------------------------------------------------------
// End of file
