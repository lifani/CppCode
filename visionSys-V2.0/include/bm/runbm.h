#ifndef __LIMIT_H__
#define __LIMIT_H__

#include <typedef.h>
#include <datatype.h>

extern const int sizeOutBuf;   /* the size of data sending to CAN */

bool RegisterBm();

bool RunBm(RECTIFIED_IMG *pIn, char* pOut, unsigned int& len, unsigned int& alarm);

bool ReleseBm();

#endif
