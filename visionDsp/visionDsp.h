#ifndef _VISION_DSP_H_
#define _VISION_DSP_H_

#include "dsp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

int InitDsp();

void vision_dsp_match(KPoint* pleft, short nl, KPoint* pright, short nr, Search_range range, short* best_choice);

void destoryDsp();

#ifdef __cplusplus
}
#endif

#endif