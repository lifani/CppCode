/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include <xml/xml.h>

#define INSERT_TAG_HANDLER(x, y) \
 m_mapHandle.insert(map<string, PASER_TAG_HANDLE>::value_type(x, y))

CXml::CXml()
{
	INSERT_TAG_HANDLER("data", &CXml::PaserDataTagNode);
	INSERT_TAG_HANDLER("MSG", &CXml::PaserMsgTagNode);
	INSERT_TAG_HANDLER("proc", &CXml::PaserProcTagNode);
	INSERT_TAG_HANDLER("option", &CXml::PaserOptionTagNode);
}

CXml::~CXml()
{
	map<long, MSG_TAG*>::iterator itm = m_mapMsg.begin();
	for (; itm != m_mapMsg.end(); ++itm)
	{
		delete itm->second;
		itm->second = NULL;
	}
}

bool CXml::LoadXml(const string& strPath)
{
	TiXmlDocument tiXmlDoc(strPath.c_str());
	
	if (!tiXmlDoc.LoadFile())
    {
        return false;
    }

	TiXmlElement* pRoot = tiXmlDoc.RootElement();

    // Parse every tag
    TiXmlElement* pChild = pRoot->FirstChildElement();
    for (; pChild != NULL; pChild = pChild->NextSiblingElement())
    {
		string name = pChild->Value();
		
		PASER_TAG_HANDLE hdl = m_mapHandle[name];
		if (PASER_TAG_HANDLE(0) != hdl)
		{
			(this->*hdl)(pChild);
		}
	}
	
	return true;
}

void CXml::GetProcTag(map<string, PROC_TAG>& mapProcTag)
{
	mapProcTag.clear();
	
	map<string, PROC_TAG>::iterator itm = m_mapProc.begin();
	for (; itm != m_mapProc.end(); ++itm)
	{
		mapProcTag[itm->first] = itm->second;
	}
}

void CXml::GetMsgTag(map<long, MSG_TAG*>& mapMsgTag)
{
	mapMsgTag.clear();
	
	map<long, MSG_TAG*>::iterator itm = m_mapMsg.begin();
	for (; itm != m_mapMsg.end(); ++itm)
	{
		mapMsgTag[itm->first] = itm->second;
	}
}

void CXml::GetOption(vector<OPTION>& vOption)
{
	vOption.clear();
	
	vector<OPTION>::iterator itv = m_vOption.begin();
	for (; itv != m_vOption.end(); ++itv)
	{
		vOption.push_back(*itv);
	}
}

int CXml::PaserDataTagNode(TiXmlElement* pElement)
{
	if (NULL == pElement)
	{
		return -1;
	}
	
	TiXmlElement* pChild = pElement->FirstChildElement();
	while (NULL != pChild)
	{
		string strImu;
		string strFunc;
		
		GetStrAttribute(pChild, "type", strImu);
		GetStrAttribute(pChild, "func", strFunc);
		
		m_mapData[strImu] = strFunc;
		
		pChild = pChild->NextSiblingElement();
	}
	
	return 0;
}

int CXml::PaserMsgTagNode(TiXmlElement* pElement)
{
	if (NULL == pElement)
	{
		return -1;
	}
	
	TiXmlElement* pChild = pElement->FirstChildElement();
	for (; NULL != pChild; pChild = pChild->NextSiblingElement())
	{
		MSG_TAG* pMsgTag = new MSG_TAG;
		if (NULL == pMsgTag)
		{
			return -1;
		}
		
		pMsgTag->id = GetIntAttribute(pChild, "ID");
		pMsgTag->begin_pos = GetIntAttribute(pChild, "begin");
		pMsgTag->off_set = GetIntAttribute(pChild, "offset");
		pMsgTag->isBig = GetIntAttribute(pChild, "isBig");
		pMsgTag->type = GetIntAttribute(pChild, "type");
		pMsgTag->next = NULL;
		
		GetStrAttribute(pChild, "data", pMsgTag->data_type);
		
		// 获取处理函数名
		GetFunc(pMsgTag);
		
		if (m_mapMsg.end() == m_mapMsg.find(pMsgTag->id))
		{
			m_mapMsg[pMsgTag->id] = pMsgTag;
		}
		else
		{
			// 添加到尾部
			AddTail(m_mapMsg[pMsgTag->id], pMsgTag);
		}
	}
	
	return 0;
}

int CXml::PaserProcTagNode(TiXmlElement* pElement)
{
	if (NULL == pElement)
	{
		return -1;
	}
	
	PROC_TAG procTag;
	
	// 添加属性值
	AddProcTagConfig(pElement, procTag);
	
	TiXmlElement* pChild = pElement->FirstChildElement();
	for (; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		PROC_TAG pTag;
		
		AddProcTagConfig(pChild, pTag);
		
		procTag.vProcTag.push_back(pTag);
	}
	
	m_mapProc[procTag.name] = procTag;
	
	return 0;
}

int CXml::PaserOptionTagNode(TiXmlElement* pElement)
{
	if (NULL == pElement)
	{
		return -1;
	}
	
	TiXmlElement* pChild = pElement->FirstChildElement();
	for (; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		OPTION op;
		
		op.key = GetIntAttribute(pChild, "value");
		
		GetStrAttribute(pChild, "key", op.key);
		
		m_vOption.push_back(op);
	}
	
	return 0;
}

int CXml::GetIntAttribute(TiXmlElement* pElement, const string& key)
{
	if (NULL == pElement)
	{
		return -1;
	}
	
	int value = 0;
	
	pElement->Attribute(key.c_str(), &value);
	
	return value;
}

int CXml::GetStrAttribute(TiXmlElement* pElement, const string& key, string& strAttribute)
{
	if (NULL == pElement)
	{
		return -1;
	}
	
	strAttribute = pElement->Attribute(key.c_str());
	
	return 0;
}

int CXml::GetFunc(MSG_TAG* pMsgTag)
{
	if (NULL == pMsgTag)
	{
		return -1;
	}
	
	pMsgTag->fun_name = m_mapData[pMsgTag->data_type];
	
	return 0;
}

int CXml::AddTail(MSG_TAG* pHead, MSG_TAG* pMsgTag)
{
	if (NULL == pHead || NULL == pMsgTag)
	{
		return -1;
	}
	
	MSG_TAG* pNext = pHead->next;
	while(NULL != pNext)
	{
		pNext = pNext->next;
	}
	
	pNext = pMsgTag;
	
	pNext->next = NULL;
	
	return 0;
}

int CXml::AddMsgTag(TiXmlElement* pElement, PROC_TAG& procTag)
{
	if (NULL == pElement)
	{
		return -1;
	}
	
	TiXmlElement* pChild = pElement->FirstChildElement();
	for (; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		int id = GetIntAttribute(pChild, "ID");
		if (-1 == id)
		{
			continue;
		}
		
		procTag.vPMsgTag.push_back(m_mapMsg[id]);
	}
	
	return 0;
}

void CXml::AddProcTagConfig(TiXmlElement* pElement, PROC_TAG& procTag)
{
	GetStrAttribute(pElement, "name", procTag.name);
		
	procTag.pid = GetIntAttribute(pElement, "pid");
	procTag.ppid = GetIntAttribute(pElement, "ppid");
	procTag.vPMsgTag.clear();
	
	AddMsgTag(pElement, procTag);
}
