/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __ABSTRACT_PACKET_H__
#define __ABSTRACT_PACKET_H__

#include <typedef.h>
#include <datatype.h>

class CAbstractPacket
{
public :

	virtual ~CAbstractPacket() {};
	
	virtual int FD(const char* identify = NULL, int op = 0) = 0;
	
	virtual int ReadFd() = 0;
	
	virtual int WriteFd() = 0;
	
	virtual void GetContent(char* ptr, int* len) = 0;
	
	virtual void SetContent(const char* ptr, int len) = 0;
	
	virtual HANDLER GetHandler(short event) = 0;
};

#endif