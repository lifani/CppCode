#ifndef TYPEDEF_H_INCLUDED
#define TYPEDEF_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>
#include <poll.h>

#include <signal.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/mman.h>

#include <iostream>

using namespace std;

typedef void* (*FUNC)(void* arg);
typedef void (*EXIT_FUNC)();

#define DEVICE_FILENAME "/dev/fpga"
#define DEVICE_SYS_POLL "/sys/devices/platform/omap2-fpga/data"

#define MAP_HEX_FILE	"./map_hex.txt"

#define MMAP_SIZE 0x00100000

#define IMG_WIDTH 320
#define IMG_HEIGHT 240
#define IMG_SIZE (IMG_WIDTH * IMG_HEIGHT)

// feature
#define FRAME_HEAD_LEN 20
#define FRAME_DATA_LEN (995 * 36)
#define FRAME_LEN (FRAME_HEAD_LEN + FRAME_DATA_LEN)

#define THREAD_COUNT 5

//#define NO_IMURECV

//#define NO_STORE

//#define NO_DSP

#define NO_MOTION

// 默认运行vision recv 和 proc两个线程
static char BIT_MASK = 0x03;

enum {
	MONITOR_WAIT = 0,
	PROC_EXIT,
	READ_EXIT
};

typedef struct tIMU
{
	float acc_x;
	float acc_y;
	float acc_z;
	
	float gyro_x;
	float gyro_y;
	float gyro_z;
	
	float press;
	
	float q0;
	float q1;
	float q2;
	float q3;
	
	tIMU()
	{
		acc_x = 0; acc_y = 0; acc_z = 0;
		gyro_x = 0; gyro_y = 0; gyro_z = 0;
		press = 0;
		q0 = 0; q1 = 0; q2 = 0; q3 = 0;
	}
} IMU;

#endif // TYPEDEF_H_INCLUDED
