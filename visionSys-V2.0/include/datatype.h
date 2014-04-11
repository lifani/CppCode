#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#define DEVICE_FILENAME 	"/dev/fpga"
#define DEVICE_RBF			"/dev/fpga0"
#define DEVICE_RBF_FILE		"/etc/firmware/fpga.rbf"
#define DEVICE_SYS_POLL		"/sys/soc-fpga/data"
#define MAP_HEX_FILE 		"/cache/map_hex.txt"
#define COMOS_FILE			"/cache/cmos_cfg.txt"
#define VELOCITY_PATH		"/data/v"
#define RECTIFIED_PATH		"/data/r"

#define CAN_ID_NORMAL		0x109
#define CAN_ID_ATTI			0x092
#define CAN_ID_MC			0x388

#define CMD_CODE_NORMAL		0x1000
#define CMD_CODE_ATTI		0x1002
#define CMD_CODE_MC			0x1009

#define MMAP_SIZE 			0x00800000

#define SOFT_VERSION		"vl300-v2.0-T001"

#define HEART_BIT			100
#define VELOCITY_ID			101
#define BM_ID				102
#define VELOCITY_BACK		103
#define BM_BACK				104
#define STORE_FRONT			105
#define STORE_BACK			106

#define DEAMON_ID			110

#define RCM_LOG				200
#define VELOCITY_LOG		201
#define BM_LOG				202

// err code

#define ERR_INTIALIZED		0

#define ERR_FPGA_UNINITED	-2


#define ERR_PROC_UNINTIED	-20

// 寄存器起始地址
const unsigned int FPGA_HREG = 0xFF220000;

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
// 报文头大小
const unsigned int HEAD_SIZE = 20;

const unsigned int FRAME_LEN = 8;

const unsigned int MSG_MEM_SIZE = 1024 * 1024;

const unsigned int MAX_UNRECV_TIMES = 3;

const unsigned int MAX_RESTART_TIMES = 3;

// 消息驱动机制
// 和定时器
// 涉及数据结构定义开始
// ********************************
class CBaseVision;
typedef void (CBaseVision::*THREAD_FUNC)();

class CCanPacket;
typedef void (CCanPacket::*PROC_HANDLER)(char* ptr, int len);

class CAbstractPacket;
typedef int (CAbstractPacket::*HANDLER)();

typedef struct _MSG_DATA
{
	char* 			ptr;
	
	union _x
	{
		unsigned 	size;
		pid_t		pid;
	} x;
	
	union _y
	{
		char		buf[64];
		int			code;
	} y;
	
} MSG_DATA;

typedef struct _VISION_MSG
{
	long 			id;
	MSG_DATA		data;
} VISION_MSG;

typedef int (CBaseVision::*MSG_FUNC)(VISION_MSG*, int, int);

typedef struct _MSG_TAG
{
	long		id;
	string 		data_type;
	string		fun_name;
	MSG_FUNC 	pf;
	int 		begin_pos;
	int 		off_set;
	int			isBig;
	int 		type; // for fpga
	
	_MSG_TAG* next;
} MSG_TAG;

typedef struct _PROC_TAG
{
	string name;
	int    pid;
	int    ppid;
	
	vector<MSG_TAG*> vPMsgTag;
	
	vector<_PROC_TAG> vProcTag;
	
	_PROC_TAG() : name(""), pid(0), ppid(0)
	{
	}
} PROC_TAG;

typedef void (CBaseVision::*VISION_PMSG)(VISION_MSG*);
typedef void (CBaseVision::*VISION_PTIMER)();

typedef struct _VISION_MSGMAP_ENTRY
{
	long			id;
	VISION_PMSG		pfn;
} VISION_MSGMAP_ENTRY;

typedef struct _VISION_MSGMAP
{
	const struct _VISION_MSGMAP* (*pfnGetBaseMap)();
	const VISION_MSGMAP_ENTRY* pEntries;
} VISION_MSGMAP;

typedef struct _VISION_TIMERMAP_ENTRY
{
	long 			timeusec;
	VISION_PTIMER	pfn;
} VISION_TIMERMAP_ENTRY;

typedef struct _VISION_TIMER
{
	long			timeusec;
	int				signo;
	pthread_t		req_tid;
	pthread_t		res_tid;
	VISION_PTIMER 	pfn;
	CBaseVision* 	pBaseVision;
} VISION_TIMER;

typedef struct _VISION_TIMERMAP
{
	const struct _VISION_TIMERMAP* (*pfnGetBaseTimerMap)();
	const VISION_TIMERMAP_ENTRY* pEntries;
} VISION_TIMERMAP;

// ********************************
// 定义结束

typedef union _U_4Q
{
	int nq;
	float q;
} U_4Q;

typedef struct _SHM_DEAMON
{
	unsigned int index;
} SHM_DEAMON;

typedef struct _PROC_INFO
{
	string 		pname;
	int   		pid;
	int			code;
	unsigned	times;
	unsigned	restart_times;
} PROC_INFO;

// pthread
typedef struct _THREAD_ENTRY
{
	CBaseVision* pBaseVision;
	THREAD_FUNC  pfn; 
	pthread_t	 tid;
} THREAD_ENTRY;

typedef struct _OPTION
{
	string key;
	int value;
} OPTION;

typedef struct tMC
{
	short pitch;
	short roll;
	short alti;
	short aileron;
	short elevator;
	short throttle;
	short rudder;
	short coretail;
	
	tMC() : pitch(0), roll(0), alti(0), aileron(0), elevator(0), throttle(0), rudder(0), coretail(0)
	{
	}
} MC;

typedef struct _IMU_DATA
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
	
	float vgx;
	float vgy;
	float vgz;
	
	float sonar;
	
	MC mc;
} IMU_DATA;

#pragma pack(1)

typedef struct _CAN_SNT_DATA
{
	unsigned short can_id;
	char* data;
} CAN_SNT_DATA;

typedef struct _tCAN_VELOCITY_DATA
{
	float dx;
	float dy;
	float dz;
	
	short vx;
	short vy;
	short vz;
	
	unsigned char uc1;
	unsigned char uc2;

	unsigned short cnt;
} CAN_VELOCITY_DATA;

typedef struct _CAN_BM_DATA
{
	unsigned char data[256];
	
	unsigned int size;
} CAN_BM_DATA;

// VISION系统运行状态
typedef struct _VISION_STATUS
{
	int code;
} VISION_STATUS;

#pragma pack()


#endif
