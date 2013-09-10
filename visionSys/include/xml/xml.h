#ifndef __XML_H__
#define __XML_H__

#include <typedef.h>

#include <tinyxml/tinyxml.h>
#include <tinyxml/tinystr.h>

typedef struct _XML_NODE
{
	string name;
	
	vector<struct _XML_NODE> vXmlNode;
	
} xml_node;

class CXml
{
public :

	CXml() {};
	
	~CXml() {};
	
	bool ReadXml(const string& strPath, vector<xml_node>& vXmlNode);
	
private :

	bool PaserTagNode(TiXmlElement* pNode, xml_node& xmlNode);
};

#endif