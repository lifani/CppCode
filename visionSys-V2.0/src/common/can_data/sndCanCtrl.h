#ifndef __SND_CAN_CTRL_H__
#define __SND_CAN_CTRL_H__

#include "abstractCanCtrl.h"
#include "queueCtrl.h"

typedef struct _SND_DATA
{
	unsigned int size;
	char szData[64];
}SND_DATA;

class CSndCanCtrl : public CAbstractCanCtrl
{
public :

	CSndCanCtrl();
	
	virtual ~CSndCanCtrl();
	
	virtual int Initialize(unsigned short canId, unsigned short cmd);
	
	virtual int Process(struct can_frame* pFrame);
	
	virtual int SetContent(char* ptr, int len, int cmd = 0);

protected :

	unsigned int m_pos;
	unsigned int m_size;
	
	SND_DATA m_sndData;
	
	CCanQueueCtrl m_sndCtrl;
};

#endif
