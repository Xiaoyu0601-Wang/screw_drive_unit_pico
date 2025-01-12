/**
 * @file FusionConvention.h
 * @author Seb Madgwick
 * @brief Earth axes convention.
 */

#ifndef _FUSION_CONVENTION_H_
#define _FUSION_CONVENTION_H_

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Earth axes convention.
 */
typedef enum
{
    FusionConventionNwu, /* North-West-Up */
    FusionConventionEnu, /* East-North-Up */
    FusionConventionNed, /* North-East-Down */
} FusionConvention;

#endif

//------------------------------------------------------------------------------
// End of file
