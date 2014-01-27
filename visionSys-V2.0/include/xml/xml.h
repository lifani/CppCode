#ifndef __XML_H__
#define __XML_H__

#include <typedef.h>
#include <datatype.h>

#include <tinyxml/tinyxml.h>
#include <tinyxml/tinystr.h>

class CXml
{
public :

	CXml() {};
	
	~CXml() {};
	
	bool ReadXml(const string& strPath, vector<PROC_CONFIG>& vProcXmlNode, vector<MSG_CONFIG>& vMsgXmlNode);
	
	bool ReadOption(const string& strPath, vector<OPTION>& vOption);
	
private :

	bool PaserProcTagNode(TiXmlElement* pNode, PROC_CONFIG& xmlNode);
	
	bool PaserMsgTagNode(TiXmlElement* pNode, vector<MSG_CONFIG>& vMsgXmlNode);
	
	bool PaserOptionTagNode(TiXmlElement* pNode, vector<OPTION>& vOption);

};

#endif