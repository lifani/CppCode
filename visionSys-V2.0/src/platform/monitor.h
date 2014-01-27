/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef _VISION_MONITOR_H_
#define _VISION_MONITOR_H_

enum {
	MONITOR_WAIT = 0,
	PROC_EXIT,
	READ_EXIT,
	ALL_EXIT
};

bool InitMonitor();

void Monitor();

void NotifyExit(int cond);

#endif
