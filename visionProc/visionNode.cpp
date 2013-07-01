#include "visionNode.h"

const int INDEX_MAX = 1024;
int   VisionNode::index = -1;
char* VisionNode::M_BUF = new char[INDEX_MAX * (sizeof(VisionNode) + sizeof(int))];

VisionNode::VisionNode() : lImage(NULL), rImage(NULL), next(NULL)
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

	if (next != NULL)
	{
		//delete next;
		next = NULL;
	}
}

VisionNode* VisionNode::Instance()
{
	++index;
	index = index == INDEX_MAX ? 0 : index;

	char* p = M_BUF + index * (sizeof(VisionNode) + sizeof(int));
	
	VisionNode* pNode = (VisionNode*)p;
	pNode->~VisionNode();

	return new(p) VisionNode;
}

bool VisionNode::InitMemory()
{
	if (NULL == M_BUF)
	{
		return false;
	}
	
	memset(M_BUF, 0, INDEX_MAX * (sizeof(VisionNode) + sizeof(int)));
	
	return true;
}

