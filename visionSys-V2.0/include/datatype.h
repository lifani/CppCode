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
#define CAN_ID_MC			0x388

#define CMD_CODE_NORMAL		0x1000
#define CMD_CODE_ATTI		0x1002
#define CMD_CODE_MC			0x1009

#define MMAP_SIZE 0x00100000

#define SOFT_VERSION		"vl300-v1.0-T002"

#define VELOCITY_ID			101
#define BM_ID				102

#define RCM_LOG				200
#define VELOCITY_LOG		201
#define BM_LOG				202

class CBaseVision;
typedef void (CBaseVision::*THREAD_FUNC)();

typedef struct _MSG_DATA
{
	char* 			ptr;
	unsigned 		size;
} MSG_DATA;

typedef struct _VISION_MSG
{
	long 			id;
	MSG_DATA		data;
} VISION_MSG;

typedef struct _PROC_CONFIG
{
	string 	name;
	long   	pid;
	long   	ppid;
	
	vector<struct _PROC_CONFIG> vProcConfig;
} PROC_CONFIG;

typedef struct _MSG_CONFIG
{
	long			id;
	unsigned int 	size;
	unsigned int 	cnt;
} MSG_CONFIG;

typedef void (CBaseVision::*VISION_PMSG)(VISION_MSG*);

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

typedef struct _PROC_INFO
{
	string pname;
	int  pid;
} PROC_INFO;

// pthread
typedef struct _THREAD_ENTRY
{
	CBaseVision* pBaseVision;
	THREAD_FUNC  pfn; 
	pthread_t	 tid;
} THREAD_ENTRY;


#endif
