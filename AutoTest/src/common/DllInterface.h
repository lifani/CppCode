// DllInterface.h
// author   :       Li Fa Ni
// Date     :       2013-02-26
// Ver      :       1.0

#ifndef _DLL_INTERFACE_H_
#define _DLL_INTERFACE_H_

#include "../../include/typedef.h"

class CDllInterface
{
public:

    CDllInterface() {};

    virtual ~CDllInterface() {};

    virtual int StartTest(const string& strIn, const string& strOut) = 0;
};

typedef CDllInterface* (*FUNC)();
extern "C" DLL_API CDllInterface* CreateInstance();
#endif