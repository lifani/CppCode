#ifndef __XML_H__
#define __XML_H__

#include <typedef.h>
#include <datatype.h>

#include <tinyxml/tinyxml.h>
#include <tinyxml/tinystr.h>

class CXml;

typedef int (CXml::*PASER_TAG_HANDLE)(TiXmlElement*);

class CXml
{
public :

	CXml();
	
	~CXml();
	
	bool LoadXml(const string& strPath);
	
	void GetProcTag(map<string, PROC_TAG>& mapProcTag);
	
	void GetMsgTag(map<long, MSG_TAG*>& mapMsgTag);
	
	void GetOption(vector<OPTION>& vOption);
	
private :
	
	int PaserDataTagNode(TiXmlElement* pElement);
	
	int PaserMsgTagNode(TiXmlElement* pElement);
	
	int PaserProcTagNode(TiXmlElement* pElement);
	
	int PaserOptionTagNode(TiXmlElement* pElement);
	
	int GetIntAttribute(TiXmlElement* pElement, const string& key);
	
	int GetStrAttribute(TiXmlElement* pElement, const string& key, string& strAttribute);
	
	int GetFunc(MSG_TAG* pMsgTag);
	
	int AddTail(MSG_TAG* pHead, MSG_TAG* pMsgTag);
	
	int AddMsgTag(TiXmlElement* pElement, PROC_TAG& procTag);
	
	void AddProcTagConfig(TiXmlElement* pElement, PROC_TAG& procTag);
	
private :

	map<string, PASER_TAG_HANDLE> m_mapHandle;

	map<string, string> m_mapData;
	map<long, MSG_TAG*> m_mapMsg;
	map<string, PROC_TAG> m_mapProc;
	
	vector<OPTION> m_vOption;
};

#endif