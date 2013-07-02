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
	
	struct can_filter rfilter[1];
	rfilter[0].can_id = 0x090;
	rfilter[0].can_mask = CAN_SFF_MASK;
	
	if (0 != setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)))
	{
		Writelog(LOG_ERR, "Set can filter fail.", __FILE__, __LINE__);
		return false;
	}
	
	P_IMU_DATA = new char[(MAX_IMU_NUM + 1) * sizeof(IMU_Package)];
	if (NULL == P_IMU_DATA)
	{
		return false;
	}
	
	memset(P_IMU_DATA, 0, (MAX_IMU_NUM + 1) * sizeof(IMU_Package));
	
	begin_pos = 0;
	end_pos = MAX_IMU_NUM * sizeof(IMU_Package);
	
	fetch_pos = -1;
	store_pos = 0;
	
	return true;
}

void* IMUCanRecv(void* arg)
{
	struct can_frame frame;
	int size_imu = sizeof(IMU_Package);
	
	bool bIsAtti = false;
	
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
				}
			}
			
			if (bIsAtti)
			{
				for (int i = 0; i < frame.can_dlc; ++i)
				{
					P_IMU_DATA[store_pos++] = frame.data[i];
				}
				
				if (store_pos % size_imu == 0)
				{
					store_pos = frame.can_dlc == 4 && CheckIsTail((char*)frame.data) ? store_pos : fetch_pos;
					
					pthread_mutex_unlock(&imu_lock);
					bIsAtti = false;
				}
				
				if (end_pos <= store_pos)
				{	
					cout << "store_pos = " << store_pos << " end_pos = " << end_pos << endl;
					store_pos = begin_pos;
				}
			}
		}
	}
}

bool GetIMU(IMU& imu)
{
	bool bSucceed = false;

	pthread_mutex_lock(&imu_lock);
	if (fetch_pos >= 0)
	{
		IMU_Package* p = (IMU_Package*)(P_IMU_DATA + fetch_pos);
		
		imu.acc_x = p->imu.acc_x;
		imu.acc_y = p->imu.acc_y;
		imu.acc_z = p->imu.acc_z;
				
		imu.gyro_x = p->imu.gyro_x;
		imu.gyro_y = p->imu.gyro_y;
		imu.gyro_z = p->imu.gyro_z;
		
		imu.press = p->imu.press;
		
		imu.q0 = p->imu.q0;
		imu.q1 = p->imu.q1;
		imu.q2 = p->imu.q2;
		imu.q3 = p->imu.q3;
		
		bSucceed = true;
	}
	pthread_mutex_unlock(&imu_lock);
	
	return bSucceed;
}

bool CheckIsAtti(char* pData)
{
	return pData[0] == 0x55 && pData[1] == 0xAA && pData[2] == 0x55 && pData[3] == 0xAA && pData[4] == 0x02 && pData[5] == 0x10;
}

bool CheckIsTail(char* pData)
{
	return pData[0] == 0x66 && pData[1] == 0xCC && pData[2] == 0x66 && pData[3] == 0xCC;
}

void WaitImuReady()
{
	struct can_frame frame;
	
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
	
	return;
}

void exit_imu_receive()
{
	imu_receive_running = false;
}

