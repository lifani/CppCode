#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#define DEVICE_FILENAME "/dev/fpga"
#define DEVICE_SYS_POLL "/sys/devices/platform/omap2-fpga/data"
#define FPGA_READY  	"/sys/class/gpio/gpio178/value"
#define MAP_HEX_FILE 	"/cache/map_hex.txt"
#define VELOCITY_PATH	"./v"
#define RECTIFIED_PATH	"./r"

#define MMAP_SIZE 0x00100000

const unsigned int QUEUE_SIZE = 10;

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
	pid_t pid;

} HeartBeat;

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
} VELOCITY_DATA;

// 矫正图像
typedef struct _RECTIFIED_IMG
{
	unsigned char lImg[IMG_SIZE];
	unsigned char rImg[IMG_SIZE];
} RECTIFIED_IMG;

// 运算结果
typedef struct _FEEDBACK_DATA
{
	unsigned int flg;
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
