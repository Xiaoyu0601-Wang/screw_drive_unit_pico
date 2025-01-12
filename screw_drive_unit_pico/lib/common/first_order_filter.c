#include "first_order_filter.h"

void lpf_init(low_pass_filter_object_t *lpf)
{
	// lpf->output = 0;
	// lpf->low_pass_tau = 700;
// LOWPASS_A           (2.0 * PRESSURE_ALT_LOWPASS_TAU / PRESSURE_ALT_LOWPASS_SAMPLE_TIME)
// LOWPASS_GX1         (1.0 / (1.0 + PRESSURE_ALT_LOWPASS_A))
// LOWPASS_GX2         (1.0 / (1.0 + PRESSURE_ALT_LOWPASS_A))
// LOWPASS_GX3         ((1.0 - PRESSURE_ALT_LOWPASS_A) / (1.0 + PRESSURE_ALT_LOWPASS_A))
}

bool lpf_calc(low_pass_filter_object_t *lpf)
{
	// short int gap=0; 
	// gap = Abs(lpf->Output-lpf->Input);
	// lpf->DynamicPara = (float)gap/(lpf->DynamicRange);
	// lpf->output = (short int)(lpf->DynamicPara*lpf->Input
	// 								+ (1-lpf->DynamicPara)*lpf->Output);
	return true;
}


