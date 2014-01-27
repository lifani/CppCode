#ifndef __MT_H__
#define __MT_H__

#include <typedef.h>
#include <datatype.h>

class CMT
{
public :

	static int Register(vector<MSG_CONFIG>& vMsgConfig, string& pname, int pid);
	
	static int Destory();
	
	static int RecvMsg(VISION_MSG* pMsg);
	
	static int SendMsg(VISION_MSG* pMsg);
	
};

#endif

