#include <can_config.h> 
#include <libsocketcan.h>
/* For can test */

#include <errno.h>
#include <getopt.h>
#include <libgen.h>

#include <limits.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "tools.h"
#include "visionImu.h"

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

static pthread_mutex_t imu_lock = PTHREAD_MUTEX_INITIALIZER; 

const int MAX_IMU_NUM = 20;

static char* P_IMU_DATA = NULL;

static int s = -1;

static int begin_pos = 0;
static int end_pos = 0;
static int fetch_pos = 0;
static int store_pos = 0;

static bool imu_receive_running = true;

static char key;

bool InitIMUCan()
{
	const int bitrate = 1000000;
	const char* interface = "can0";
	
	if (can_do_stop(interface) < 0)
	{
		Writelog(LOG_ERR, "Can't stop can", __FILE__, __LINE__);
		return false;
	}

	if (can_set_bitrate(interface, bitrate) < 0)
	{
		Writelog(LOG_ERR, "Set can bit rate fail.", __FILE__, __LINE__);
		return false;
	}
	
	if (can_do_start(interface) < 0)
	{
		Writelog(LOG_ERR, "Fail to start can.", __FILE__, __LINE__);
		return false;
	}
	
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{
		Writelog(LOG_ERR, "Socket Fail.", __FILE__, __LINE__);
		return false;
	}

	struct ifreq ifr;
	strncpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name));
	
	if (ioctl(s, SIOCGIFINDEX, &ifr))
	{
		Writelog(LOG_ERR, "ioctl fail.", __FILE__, __LINE__);
		return false;
	}
	
	struct sockaddr_can addr;
	addr.can_family = PF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
	{
		Writelog(LOG_ERR, "bind socket fail.", __FILE__, __LINE__);
		return false;
	}
	
	P_IMU_DATA = new char[(MAX_IMU_NUM + 1) * sizeof(imu_body)];
	if (NULL == P_IMU_DATA)
	{
		return false;
	}
	
	memset(P_IMU_DATA, 0, (MAX_IMU_NUM + 1) * sizeof(imu_body));
	
	begin_pos = 0;
	end_pos = MAX_IMU_NUM * sizeof(imu_body);
	
	fetch_pos = -1;
	store_pos = 0;
	
	return true;
}

bool SetFilter(int filter)
{
	struct can_filter rfilter[1];
	rfilter[0].can_id = filter;
	rfilter[0].can_mask = CAN_SFF_MASK;
	
	if (0 != setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)))
	{
		Writelog(LOG_ERR, "Set can filter fail.", __FILE__, __LINE__);
		return false;
	}
	
	return true;
}

void* IMUCanRecv(void* arg)
{
	struct can_frame frame;
	int size_imu = sizeof(imu_body);
	
	bool bIsAtti = false;
	
	if (!SetFilter(0x108))
	{
		return NULL;
	}
	
	GetKey();
	
	if (!SetFilter(0x090))
	{
		return NULL;
	}
	
	WaitImuReady();
	
	while (imu_receive_running)
	{
		if (read(s, &frame, sizeof(struct can_frame)) > 0)
		{
			if (!bIsAtti && frame.can_dlc == 8)
			{
				bIsAtti = CheckIsAtti((char*)frame.data);
				if (bIsAtti)
				{
					pthread_mutex_lock(&imu_lock);
					fetch_pos = store_pos;
					
					continue;
				}
			}
			
			if (bIsAtti)
			{
				if (frame.can_dlc == 8)
				{
					if (CheckIsHead((char*)frame.data) || CheckIsTail((char*)(frame.data + 4)))
					{
						store_pos = fetch_pos;
						bIsAtti = false;
						pthread_mutex_unlock(&imu_lock);
					}
					else 
					{
						for (int i = 0; i < frame.can_dlc; ++i)
						{
							P_IMU_DATA[store_pos++] = frame.data[i] ^ key;
						}
					}
				}
				else
				{
					if (frame.can_dlc == 4)
					{
						if (store_pos % size_imu != 0 || !CheckIsTail((char*)frame.data))
						{
							store_pos = fetch_pos;
						}
					}
					else
					{
						store_pos = fetch_pos;
					}

					bIsAtti = false;
					pthread_mutex_unlock(&imu_lock);
				}
				
				if (end_pos <= store_pos)
				{	
					//cout << "store_pos = " << store_pos << " end_pos = " << end_pos << endl;
					store_pos = begin_pos;
				}
			}
		}
	}
	
	return NULL;
}

bool GetIMU(IMU& imu)
{
	bool bSucceed = false;

	pthread_mutex_lock(&imu_lock);
	if (fetch_pos >= 0)
	{
		imu_body* p = (imu_body*)(P_IMU_DATA + fetch_pos);
		
		imu.acc_x = p->acc_x;
		imu.acc_y = p->acc_y;
		imu.acc_z = p->acc_z;
				
		imu.gyro_x = p->gyro_x;
		imu.gyro_y = p->gyro_y;
		imu.gyro_z = p->gyro_z;
		
		imu.press = p->press;
		
		imu.q0 = p->q0;
		imu.q1 = p->q1;
		imu.q2 = p->q2;
		imu.q3 = p->q3;
		
		bSucceed = true;
	}
	pthread_mutex_unlock(&imu_lock);
	
	return bSucceed;
}

bool CheckIsAtti(char* pData)
{
	return pData[0] == 0x55 && pData[1] == 0xAA && pData[2] == 0x55 && pData[3] == 0xAA && pData[4] == 0x02 && pData[5] == 0x10;
}

bool CheckIsMcMode(char* pData)
{
	return pData[0] == 0x55 && pData[1] == 0xAA && pData[2] == 0x55 && pData[3] == 0xAA && pData[4] == 0x00 && pData[5] == 0x10;
}

bool CheckIsHead(char* pData)
{
	return pData[0] == 0x55 && pData[1] == 0xAA && pData[2] == 0x55 && pData[3] == 0xAA;
}

bool CheckIsTail(char* pData)
{
	return pData[0] == 0x66 && pData[1] == 0xCC && pData[2] == 0x66 && pData[3] == 0xCC;
}

void GetKey()
{
	struct can_frame frame;

	int size = sizeof(NormalMode);
	bool bIs = false;
	int cnt = 0;
	
	NormalMode tMode;
	char* pMode = (char*)&tMode;
	
	while (read(s, &frame, sizeof(can_frame)) > 0)
	{
		if (!bIs && frame.can_dlc == 8)
		{
			bIs = CheckIsMcMode((char*)frame.data);
			cnt = 0;
		}
		
		if (bIs)
		{
			if (cnt + frame.can_dlc > size)
			{
				bIs = false;
				cnt = 0;
			}
			else
			{
				for (int i = 0; i < frame.can_dlc; ++i)
				{
					*(pMode + cnt + i) = frame.data[i];
				}
				
				cnt += frame.can_dlc;
			}
			
			if (cnt == size)
			{
				if (CheckIsTail((char*)frame.data + 4))
				{
					break;
				}
				else
				{
					cnt = 0;
					bIs = false;
				}
			}
		}
	}
	
	key = tMode.mode.key;
	
	printf("key = %x ", key);
}

void WaitImuReady()
{
	struct can_frame frame;
	
	cout << "wait imu ready..." << endl;

	int size_imu = sizeof(IMU_Package);
	bool bIsAtti = false;
	int cnt = 0;
	
	
	while (read(s, &frame, sizeof(can_frame)) > 0)
	{
		if (!bIsAtti && frame.can_dlc == 8)
		{
			bIsAtti = CheckIsAtti((char*)frame.data);
			cnt = 0;
		}
		
		if (bIsAtti)
		{
			cnt += frame.can_dlc;
		}
		
		if (bIsAtti && frame.can_dlc == 4)
		{
			if (CheckIsTail((char*)frame.data))
			{
				if (cnt == size_imu)
				{
					break;
				}
				else
				{
					cnt = 0;
					bIsAtti = false;
				}
			}
		}
	}

	cout << "imu ready." << endl;
	return;
}

void exit_imu_receive()
{
	imu_receive_running = false;
}

