#include "visionNode.h"

int   VisionNode::index = 0;
char* VisionNode::M_BUF = new char[INDEX_MAX * (sizeof(VisionNode) + sizeof(int))];

VisionNode::VisionNode() : lImage(NULL), rImage(NULL), imu(NULL), next(NULL)
{
}

VisionNode::~VisionNode()
{
    if (lImage != NULL)
    {
        delete []lImage;
        lImage = NULL;
    }

    if (rImage != NULL)
    {
        delete []rImage;
        rImage = NULL;
    }

    if (imu != NULL)
    {
        delete []imu;
        imu = NULL;
    }

    if (next != NULL)
    {
        delete next;
        next = NULL;
    }
}

VisionNode* VisionNode::Instance()
{
    ++index;
    index = index == INDEX_MAX ? 0 : index;

    char* p = M_BUF + index * (sizeof(VisionNode) + sizeof(int));

    return new(p) VisionNode;
}
