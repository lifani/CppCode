/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.15
*************************************/
#ifndef __VISION_STORE_H__
#define __VISION_STORE_H__

#include <typedef.h>
#include <datatype.h>

#include "queueCtrl.h"

typedef struct _STORE_ENTRY
{
	unsigned char data[IMG_SIZE];
	unsigned int size;
} STORE_ENTRY;

typedef struct _STORE_NODE
{
	STORE_ENTRY entries[4];
	unsigned int cnt;
	
	IMU_DATA imu;
} STORE_NODE;

class CVisionStore
{
public :
	
	CVisionStore(const char* path, 
		const unsigned int folders, const unsigned int files);
	
	~CVisionStore();
	
	int Initialize();
	
	int Out(STORE_NODE* node);
	
private :

	int ExecCmd(const char* strCmd);
	
	int CreateDir(const char* path);
	
	int OutImg(STORE_NODE* node);
	
	int OutImu(IMU_DATA& imu);

private :

	string m_ppath;
	string m_path;
	
	int m_imufd;
	
	unsigned int m_fIndex;
	unsigned int m_dIndex;
	unsigned int m_folders;
	unsigned int m_files;
};

#endif
