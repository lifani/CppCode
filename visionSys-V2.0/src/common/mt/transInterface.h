/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __TRANS_INTERFACE_H__
#define __TRANS_INTERFACE_H__

#undef LOG_TAG
#define LOG_TAG "VISION_MT"

class CTransInterface
{
public :
	
	virtual ~CTransInterface() {};
	
	virtual int ReadMsg(VISION_MSG* pMsg) = 0;
	
	virtual int WriteMsg(VISION_MSG* pMsg) = 0;
};

#endif
