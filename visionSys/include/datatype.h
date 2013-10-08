#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#define DEVICE_FILENAME "/dev/fpga"
#define DEVICE_SYS_POLL "/sys/devices/platform/omap2-fpga/data"
#define FPGA_READY  	"/sys/class/gpio/gpio178/value"
#define FPGA_CONIFG_OK	"/sys/class/gpio/gpio165/value" 
#define MAP_HEX_FILE 	"/cache/map_hex.txt"
#define COMOS_FILE		"/cache/cmos_cfg.txt"
#define VELOCITY_PATH	"/data/v"
#define RECTIFIED_PATH	"/data/r"

#define CAN_ID_NORMAL		0x108
#define CAN_ID_ATTI			0x090

#define CMD_CODE_NORMAL		0x1000
#define CMD_CODE_ATTI		0x1002

#define MMAP_SIZE 0x00100000

enum
{
	IS_VELOCITY,
	IS_BM
};

const unsigned int QUEUE_SIZE = 10;

const unsigned int STORE_QUUE_SIZE = 100;

// img cols
const unsigned int IMG_WIDTH = 320;

// img rows
const unsigned int IMG_HEIGHT = 240;

// img size
const unsigned int IMG_SIZE = IMG_WIDTH * IMG_HEIGHT;

// 最大点数
const unsigned int MAX_CLOUD_CNT = 995;

// 点大小
const unsigned int POINT_LEN = 36;

// 点云大小
const unsigned int MAX_CLOUD_SIZE = 38400;

const unsigned int HEAD_SIZE = 20;

// 子进程信息
typedef struct _PROC_INFO
{
	string pname;
	pid_t pid;
	int times;
} PROC_INFO;

// 心跳信息
typedef struct _HeartBeat
{
	long  type;
} HeartBeat;

#pragma pack(1)
typedef struct _CAN_VELOCITY_DATA
{
	unsigned char uc1;
	unsigned char uc2;
	
	unsigned short vx;
	unsigned short vy;
	
	float dxy;
	float dyz;
	float dxz;
	
	unsigned short cnt;
} CAN_VELOCITY_DATA;

typedef struct _CAN_BM_DATA
{
	unsigned char data[144];	
} CAN_BM_DATA;

#pragma pack()

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

typedef struct _NormalMode
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
} NormalMode;

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

typedef struct _SHM_DATA
{
	char data[256];
} SHM_DATA;

// 点云
typedef struct _VELOCITY_DATA
{
	unsigned short lcnt;
	unsigned char lCloud[MAX_CLOUD_SIZE];
	
	unsigned short rcnt;
	unsigned char rCloud[MAX_CLOUD_SIZE];
	
	IMU imu;
	
	unsigned int index;
} VELOCITY_DATA;

// 矫正图像
typedef struct _RECTIFIED_IMG
{
	unsigned char lImg[IMG_SIZE];
	unsigned char rImg[IMG_SIZE];
	
	unsigned int index;
} RECTIFIED_IMG;

// 飞控数据
typedef struct _FIGHTCTL_DATA
{
	unsigned int type;
	unsigned char data[IMG_SIZE];
} FIGHTCTL_DATA;

// 运算结果
typedef struct _FEEDBACK_DATA
{
	unsigned int flg;
	unsigned int cnt;
	unsigned char data[IMG_SIZE];
} FEEDBACK_DATA;

typedef void (*FUNC) (union sigval val);

class CBaseVision;

typedef void (CBaseVision::*PFUNC)();

typedef struct _PTHREAD_PFUNC
{
	CBaseVision* pBaseVision;
	PFUNC	pFunc;
	pthread_t tid;
} PTHREAD_PFUNC;

#endif
