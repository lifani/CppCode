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
	for ( int i = 0; i < MAXNUMOFCANID; ++i )
	{
		m_packet[i] = NULL;
	}
}

can_data::~can_data()
{
	for ( int i = 0; i < MAXNUMOFCANID; ++i )
	{
		delete m_packet[i];
		m_packet[i] = NULL;
	}
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

Packet* can_data::Read()
{
	struct pollfd poll_fd;
	poll_fd.fd = m_can_sck;
	poll_fd.events = POLLIN;
	
	struct can_frame frame;
	Packet *pPacket = NULL;
	
	bool bIsOk = false;
	while( !bIsOk )
	{
		while( true )
		{
			if (poll(&poll_fd, 1, -1) > 0)
			{
				if ( read(m_can_sck, &frame, sizeof(struct can_frame)) < 0 )
				{
					usleep(2000);
				}
				else
				{
					break;
				}
			}
			else
			{
				usleep(2000);
			}
		}

		unsigned short usCanId = frame.can_id;
		for ( int nIndex = 0; nIndex < MAXNUMOFCANID; ++nIndex )
		{
			pPacket = m_packet[nIndex];
			
			if (NULL != pPacket && pPacket->IsEqualFilter(usCanId))
			{
				if ( pPacket->Push( frame ) )
				{
					bIsOk = true;
					break;
				}
			}
		}
	}
	
	return pPacket;
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
		snd_size += 2; //末尾有2个字节的checksum

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

		while ( true )
		{
			if (poll(&poll_fd, 1, -1) > 0)
			{
				if ( write(m_can_sck, &frame, sizeof(struct can_frame)) < 0 )
				{
					usleep(2000);
				}
				else
				{
					break;
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

bool can_data::SetFilter( vector<Filter_param>& rFilterVector )
{
	// reset
	for ( int nCurIndex = 0; nCurIndex < MAXNUMOFCANID; ++nCurIndex )
	{
		if ( NULL == m_packet[nCurIndex] )
		{
			break;
		}
		
		delete m_packet[nCurIndex];
		m_packet[nCurIndex] = NULL;
	}

	struct can_filter rfilter[MAXNUMOFCANID];
	for ( unsigned int nIndex = 0; nIndex < rFilterVector.size(); ++nIndex )
	{
		unsigned short usCanId = rFilterVector[nIndex].m_usCanId;
		unsigned short usCmdCode = rFilterVector[nIndex].m_usCmdCode;
		
		rfilter[nIndex].can_id = usCanId;
		rfilter[nIndex].can_mask = CAN_SFF_MASK;

		Packet* p = new Packet( usCanId, usCmdCode );
		if (NULL != p)
		{
			if (usCanId != CAN_ID_MC)
			{
				p->SetKey(m_key);
			}
			
			m_packet[nIndex] = p;
		}
		
	}
	
	if (0 != setsockopt(m_can_sck, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(can_filter)*rFilterVector.size()))
	{
		return false;
	}
	
	return true;
}

void can_data::SetKey(unsigned char key)
{
	m_key = key;
	//Packet::SetKey( key );
}
