#include "first_order_filter.h"

////////////////////////////////////////
// TAU = Filter Time Constant
// T   = Filter Sample Time
// A   = 2 * TAU / T

// Low Pass:
// GX1 = 1 / (1 + A)
// GX2 = 1 / (1 + A)
// GX3 = (1 - A) / (1 + A)

// High Pass:
// GX1 =  A / (1 + A)
// GX2 = -A / (1 + A)
// GX3 = (1 - A) / (1 + A)
///////////////////////////////////////

void low_pass_filter_init(first_order_filter_object_t *lpf)
{
	lpf->previous_input = 0.0f;
	lpf->previous_output = 0.0f;
	lpf->gain = (2.0f * lpf->first_order_tau / lpf->first_order_sample_time);
	lpf->gx1 = (1.0f / (1.0f + lpf->gain));
	lpf->gx2 = (1.0f / (1.0f + lpf->gain));
	lpf->gx3 = ((1.0f - lpf->gain) / (1.0f + lpf->gain));
}

bool low_pass_filter_calc(float input, first_order_filter_object_t *lpf)
{
	lpf->previous_output = lpf->gx1 * input +
            			   lpf->gx2 * lpf->previous_input -
             			   lpf->gx3 * lpf->previous_output;

    lpf->previous_input  = input;

	return true;
}

