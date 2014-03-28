#ifndef __MT_H__
#define __MT_H__

#include <typedef.h>
#include <datatype.h>

class CMT
{
public :

	static int Register(const map<long, MSG_TAG*>& mapPMsgTag, string& pname, int pid);
	
	static int Destory();
	
	static int RecvMsg(VISION_MSG* pMsg);
	
	static int SendMsg(VISION_MSG* pMsg);
	
};

#endif

