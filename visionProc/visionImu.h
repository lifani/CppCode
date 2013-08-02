#ifndef _VISION_IMU_H_
#define _VISION_IMU_H_

#include "typedef.h"

#pragma pack(1)
typedef struct cmd_imu_body
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
} imu_body;

typedef struct tIMU_Package
{
	char header[4];
	short code;
	short len;
	
	imu_body imu;
	
	char endflag[4];
} IMU_Package;

struct cmd_select_normal_mode
{
	float imu_offset_x;
	float imu_offset_y;
	float imu_offset_z;
	
	float gps_offset_x;
	float gps_offset_y;
	float gps_offset_z;
	
	short res0;
	char  key;
	char  res1;
	
	float compass_bias_x;
	float compass_bias_y;
	float compass_bias_z;
	
	float compass_scale_x;
	float compass_scale_y;
	float compass_scale_z;
};

typedef struct tNormalMode
{
	char header[4];
	short code;
	short len;

	struct cmd_select_normal_mode mode;
	
	char endflag[4];
} NormalMode;

#pragma pack()

bool InitIMUCan();

void* IMUCanRecv(void* arg);

bool GetIMU(IMU& imu);

static bool CheckIsAtti(char* pData);

static bool CheckIsMcMode(char* pData);

static bool CheckIsHead(char* pData);
static bool CheckIsTail(char* pData);

static void GetKey();
static void WaitImuReady();

static bool SetFilter(int filter);

void exit_imu_receive();

#endif