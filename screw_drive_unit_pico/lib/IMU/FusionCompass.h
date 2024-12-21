/**
 * @file FusionCompass.h
 * @author Seb Madgwick
 * @brief Tilt-compensated compass to calculate the magnetic heading using
 * accelerometer and magnetometer measurements.
 */

#ifndef _FUSION_COMPASS_H_
#define _FUSION_COMPASS_H_

//------------------------------------------------------------------------------
// Includes

#include "FusionConvention.h"
#include "FusionMath.h"

//------------------------------------------------------------------------------
// Function declarations

float FusionCompassCalculateHeading(const FusionConvention convention, const FusionVector accelerometer,
                                    const FusionVector magnetometer);

#endif

//------------------------------------------------------------------------------
// End of file
