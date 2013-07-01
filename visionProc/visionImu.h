#ifndef _VISION_IMU_H_
#define _VISION_IMU_H_

#include "typedef.h"

#pragma pack(1)
struct cmd_imu_body
{
	double 	longti;
	double 	lati;
	float	alti;
	
	float acc_x;
	float acc_y;
	float acc_z;
	
	float gyro_x;
	float gyro_y;
	float gyro_z;
	
	float	press;
	
	float	q0;
	float	q1;
	float	q2;
	float 	q3;
	
	float	agx;
	float	agy;
	float	agz;
	
	float	vgx;
	float	vgy;
	float	vgz;
	
	float	gbx;
	float	gby;
	float	gbz;
	
	short	mx;
	short	my;
	short	mz;
	short	temp[3];
	
	unsigned short sensor_overflow;
	unsigned short filter_status;
	unsigned short gps_svn;
	unsigned short cnt;
};

typedef struct tIMU_Package
{
	char header[4];
	short code;
	short len;
	
	struct cmd_imu_body imu;
	
	char endflag[4];
} IMU_Package;

#pragma pack()

bool InitIMUCan();

void* IMUCanRecv(void* arg);

bool GetIMU(IMU& imu);

static bool CheckIsAtti(char* pData);

static bool CheckIsTail(char* pData);

static void WaitImuReady();

void exit_imu_receive();

#endif