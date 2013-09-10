#include <can_config.h> 
#include <libsocketcan.h>

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

#include "can_data.h"

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

can_interface* CreateCanInterface(const char* can_name, unsigned short can_id, unsigned short cmd_code, unsigned int size)
{
	if (NULL == can_name)
	{
		return NULL;
	}
	
	can_interface* p = new can_data(can_name, can_id, cmd_code, size);
	if (NULL != p)
	{
		if (!p->Register())
		{
			delete p;
			p = NULL;
		}
	}
	
	return p;
}

can_data::can_data(const char* can_name, unsigned short can_id, unsigned short cmd_code, unsigned int size)
: m_can_name(can_name)
, m_can_id(can_id)
, m_cmd_code(cmd_code)
, m_size(size)
, m_bitrate(1000000)
, m_key(0)
, m_can_sck(-1)
{
}

can_data::~can_data()
{
}

bool can_data::Register()
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
	
	SetFilter(m_can_id, m_cmd_code);
	
	return true;
}

int can_data::Read(char* ptr, unsigned short* len)
{
	if (*len < m_size)
	{
		return -1;
	}
	
	struct can_frame frame;
	
	bool IsOk 	= false;
	bool bBegin = false;
	
	unsigned int pos = 0;
	
	while (!IsOk)
	{
		while (!bBegin)
		{
			if (read(m_can_sck, &frame, sizeof(struct can_frame)) > 0)
			{
				if (IsHead(frame))
				{
					pos = 0;
					*len = *(unsigned short*)((char*)frame.data + 6);
					
					bBegin = true;
				}
			}
		}
		
		while (bBegin && !IsOk)
		{
			if (read(m_can_sck, &frame, sizeof(struct can_frame)) > 0)
			{
				int i = 0;
				for (; i < frame.can_dlc && pos < *len; ++i, ++pos)
				{
					*(ptr + pos) = frame.data[i] ^ m_key;
				}
				
				if (pos == *len && i < frame.can_dlc)
				{
					if (IsTail(frame, i))
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

int can_data::Write(const char* ptr, unsigned int len)
{
	if (NULL == ptr)
	{
		return -1;
	}
	
	struct can_frame frame;
	frame.can_id = m_can_id;
	frame.can_id &= CAN_SFF_MASK;

	can_head head = {{0x55, 0xaa, 0x55, 0xaa}, m_cmd_code, len};
	
	// ����֡ͷ
	memcpy((char*)frame.data, (char*)&head, sizeof(can_head));
	frame.can_dlc = sizeof(can_head);
	
	if (write(m_can_sck, &frame, sizeof(struct can_frame)) == -1)
	{
		cout << "send can head error." << endl;
		return -1;
	}
	
	// �������ݶ�
	unsigned int pos = 0;
	unsigned int dlc = 0;
	while (pos < len)
	{
		for (dlc = 0; dlc < 8 && pos < len; ++dlc, ++pos)
		{
			frame.data[dlc] = *(ptr + pos);
		}
		
		frame.can_dlc = dlc;
		
		if (write(m_can_sck, &frame, sizeof(struct can_frame)) == -1)
		{
			cout << "send can data error." << endl;
			return -1;
		}
	}
	
	// ����֡β
	memcpy((char*)frame.data, can_tail, sizeof(can_tail));
	frame.can_dlc = sizeof(can_tail);
	
	if (write(m_can_sck, &frame, sizeof(struct can_frame)) == -1)
	{
		cout << "send can tail error." << endl;
		return -1;
	}

	return len;
}

bool can_data::ResetFilter(unsigned short can_id, unsigned short cmd_code)
{
	return SetFilter(can_id, cmd_code);
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