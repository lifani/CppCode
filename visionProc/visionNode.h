#ifndef VISIONNODE_H_INCLUDED
#define VISIONNODE_H_INCLUDED

#include "typedef.h"

class VisionNode
{
public:

    static VisionNode* Instance();

    static void destroy(VisionNode* p)
    {
        p->~VisionNode();
    }

private:

   VisionNode();

   ~VisionNode();

public:

    char* lImage;
    char* rImage;
    char* imu;

    VisionNode* next;

    static int index;
    static char* M_BUF;
};


#endif // VISIONNODE_H_INCLUDED
