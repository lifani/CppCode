#include <can_config.h> 
#include <libsocketcan.h>

#include <errno.h>
#include <getopt.h>
#include <libgen.h>

#include <poll.h>
#include <limits.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <errno.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "can_data.h"
#include "crc16.h"
#include "MF_CRC8.h"

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

can_interface* CreateCanInterface(const char* can_name)
{
	if (NULL == can_name)
	{
		return NULL;
	}
	
	can_interface* p = new can_data(can_name);
	if (NULL != p)
	{
		if (!p->Init())
		{
			delete p;
			p = NULL;
		}
	}
	
	return p;
}

can_data::can_data(const char* can_name)
: m_can_name(can_name)
, m_can_id(0)
, m_cmd_code(0)
, m_bitrate(1000000)
, m_key(0)
, m_can_sck(-1)
, m_bIsOldProtocal(false)
{
	m_SndData[0] = 0x55;
}

can_data::~can_data()
{
}

bool can_data::Init()
{
	if (can_do_stop(m_can_name.c_str()) < 0)
	{
		cout << "stop can error." << endl;
		return false;
	}
	
	if (can_set_bitrate(m_can_name.c_str(), m_bitrate) < 0)
	{
		cout << "set bitrate error." << endl;
		return false;
	}
	
	if (can_do_start(m_can_name.c_str()) < 0)
	{
		cout << "start can error." << endl;
		return false;
	}
	
	if ((m_can_sck = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{
		cout << "socket fail." << endl;
		return false;
	}

	struct ifreq ifr;
	strncpy(ifr.ifr_name, m_can_name.c_str(), sizeof(ifr.ifr_name));
	
	if (ioctl(m_can_sck, SIOCGIFINDEX, &ifr))
	{
		cout << "ioctl fail." << endl;
		return false;
	}
	
	struct sockaddr_can addr;
	addr.can_family = PF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(m_can_sck, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
	{
		cout << "bind socket fail." << endl;
		return false;
	}
	
	return true;
}

int can_data::Read(char* ptr, unsigned short size, int type)
{	
	struct can_frame frame;
	
	bool IsOk 	= false;
	bool bBegin = false;
	
	unsigned int pos = 0;
	
	unsigned short len = 0;
	while (!IsOk)
	{
		while (!bBegin)
		{
			if (read(m_can_sck, &frame, sizeof(struct can_frame)) > 0)
			{
				unsigned short code = *(unsigned short*)(frame.data + 4);
				
				if (IsHead(frame) && code == m_cmd_code)
				{
					pos = 0;
					len = *(unsigned short*)((char*)frame.data + 6);
					
					bBegin = true;
				}
			}
		}
		
		while (bBegin && !IsOk)
		{
			if (read(m_can_sck, &frame, sizeof(struct can_frame)) > 0)
			{
				int i = 0;
				for (; i < frame.can_dlc && pos < len; ++i, ++pos)
				{
					*(ptr + pos) = frame.data[i] ^ m_key;
				}
				
				if (pos == len && i < frame.can_dlc)
				{
					if (IsTail(frame, i) && (1 != type || Check4Q(ptr)))
					{
						IsOk = true;
					}
					else
					{
						pos = 0;
						bBegin = false;
					}
				}
			}
		}
	}
	
	return pos;
}

void can_data::SetProtocal( bool bIsOldProtocal )
{
	m_bIsOldProtocal = bIsOldProtocal;
}

int can_data::Write(const char* ptr, unsigned int len, unsigned short can_id, unsigned short cmd_code)
{
	if (NULL == ptr)
	{
		return -1;
	}
	
	if (0 == can_id)
	{
		can_id = m_can_id;
	}
	
	if (0 == cmd_code)
	{
		cmd_code = m_cmd_code;
	}
	
	struct can_frame frame;
	frame.can_id = can_id;
	frame.can_id &= CAN_SFF_MASK;

	unsigned short snd_size = 0;
	if ( !m_bIsOldProtocal )
	{
		m_SndData[0] = 0x55;
		unsigned short uLen = (unsigned short)(len + 6);
		unsigned short *pLen = (unsigned short*)(m_SndData + 1 );
		*pLen = uLen;
		snd_size = 4;
	
		// 数据
		memcpy(m_SndData + snd_size, ptr, len);
		snd_size += len;
		snd_size += 2;//末尾有2个字节的checksum

		Append_CRC8_Check_Sum( (unsigned char*)m_SndData, 4 );
		Append_CRC16_Check_Sum( (unsigned char*)m_SndData, (unsigned int)uLen );
	}
	else
	{
		// 帧头
		can_head head = {{0x55, 0xaa, 0x55, 0xaa}, cmd_code, len};
		memcpy(m_SndData + snd_size, (char*)&head, sizeof(can_head));
		snd_size = sizeof(can_head);
		
		// 数据
		memcpy(m_SndData + snd_size, ptr, len);
		snd_size += len;
		
		// 帧尾
		memcpy(m_SndData + snd_size, can_tail, sizeof(can_tail));
		snd_size += sizeof(can_tail);
	}

	// pollfd
	struct pollfd poll_fd;
	poll_fd.fd = m_can_sck;
	poll_fd.events = POLLOUT;
	
	// 发送帧
	unsigned int pos = 0;
	unsigned int dlc = 0;
	while (pos < snd_size)
	{
		for (dlc = 0; dlc < 8 && pos < snd_size; ++dlc, ++pos)
		{
			frame.data[dlc] = *(m_SndData + pos);
		}
		
		frame.can_dlc = dlc;

		while(write(m_can_sck, &frame, sizeof(struct can_frame)) < 0)
		{
			if (ENOBUFS == errno)
			{
				if (poll(&poll_fd, 1, -1) > 0)
				{
					continue;
				}
				else
				{
					usleep(2000);
				}
			}
			else
			{
				usleep(2000);
			}
		}
	}

	return len;
}

bool can_data::SetFilter(unsigned short can_id, unsigned short cmd_code)
{
	struct can_filter rfilter[1];
	rfilter[0].can_id = can_id;
	rfilter[0].can_mask = CAN_SFF_MASK;
	
	if (0 != setsockopt(m_can_sck, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)))
	{
		cout << "set can filter fail." << endl;
		return false;
	}
	
	m_cmd_code = cmd_code;
	
	return true;
}

void can_data::SetKey(unsigned char key)
{
	m_key = key;
}

inline bool can_data::IsHead(struct can_frame& frame)
{
	bool bRet = false;
	
	if (frame.can_dlc == 8)
	{
		char* pData = (char*)frame.data;
		bRet = pData[0] == 0x55 && pData[1] == 0xaa && pData[2] == 0x55 && pData[3] == 0xaa;
	}
	
	return bRet;
}

inline bool can_data::IsTail(struct can_frame& frame, int pos)
{
	bool bRet = false;
	
	if (frame.can_dlc - pos >= 4)
	{
		char* pData = (char*)frame.data + (frame.can_dlc - 4);
		bRet = pData[0] == 0x66 && pData[1] == 0xcc && pData[2] == 0x66 && pData[3] == 0xcc;
	}
	
	return bRet;
}

inline bool can_data::IsNeed(struct can_frame& frame)
{
	bool bRet = false;
	
	if (frame.can_dlc == 8)
	{
		unsigned short* p_cmd_code = (unsigned short*)((char*)frame.data + 4);
		bRet = (*p_cmd_code == m_cmd_code);
	}
	
	return bRet;
}

bool can_data::Check4Q(char* pData)
{
	if (NULL == pData)
	{
		return false;
	}
	
	imu_body* p = (imu_body*)pData;
	
	if (p->q0 >= 1.0 || p->q1 >= 1.0 || p->q2 >= 1.0 || p->q3 >= 1.0)
	{
		return false;
	}
	
	if (p->q0 <= -1.0 || p->q1 <= -1.0 || p->q2 <= -1.0 || p->q3 <= -1.0)
	{
		return false;
	}
	
	float t = p->q0 * p->q0 + p->q1 * p->q1 + p->q2 * p-> q2 + p->q3 * p->q3;
	if (t >= 0.95 && t <= 1.05)
	{
		return true;
	}
	
	return false;
}

