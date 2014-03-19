/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include <xml/xml.h>

bool CXml::ReadXml(const string& strPath, vector<PROC_CONFIG>& vProcXmlNode, 
	vector<MSG_CONFIG>& vMsgXmlNode)
{
	vProcXmlNode.clear();
	vMsgXmlNode.clear();
	
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
		if (name.compare(string("proc")) == 0)
		{
			PROC_CONFIG xmlNode;
			if (!PaserProcTagNode(pChild, xmlNode))
			{
				return false;
			}
			
			vProcXmlNode.push_back(xmlNode);
		}

        if (name.compare(string("msg")) == 0)
		{
			PaserMsgTagNode(pChild, vMsgXmlNode);
		}
    }

	
	return true;
}

bool CXml::ReadOption(const string& strPath, vector<OPTION>& vOption)
{
	vOption.clear();
	
	TiXmlDocument tiXmlDoc(strPath.c_str());
	if (!tiXmlDoc.LoadFile())
	{
		return false;
	}
	
	TiXmlElement* pRoot = tiXmlDoc.RootElement();
	
	TiXmlElement* pChild = pRoot->FirstChildElement();
	for (; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		string name = pChild->Value();
		if (name.compare(string("option")) == 0)
		{
			PaserOptionTagNode(pChild, vOption);
		}
	}
	
	return true;
}

bool CXml::PaserProcTagNode(TiXmlElement* pNode, PROC_CONFIG& xmlNode)
{
	if (NULL == pNode)
    {
        return false;
    }

	// name
	xmlNode.name = pNode->Attribute("name");
	
	int value = 0;
	pNode->Attribute("pid", &value);
	xmlNode.pid = value;
	
	pNode->Attribute("ppid", &value);
	xmlNode.ppid = value;
	
	vector<PROC_CONFIG>& vProcConfig = xmlNode.vProcConfig;
	
    // Read node
    TiXmlElement* pChild = pNode->FirstChildElement();
    for (; pChild != NULL; pChild = pChild->NextSiblingElement())
    {
        PROC_CONFIG node;
		node.name = pChild->Attribute("name");

		pChild->Attribute("pid", &value);
		node.pid = value;
		
		pChild->Attribute("ppid", &value);
		node.ppid = value;
		
        vProcConfig.push_back(node);
    }

    return true;
}

bool CXml::PaserMsgTagNode(TiXmlElement* pNode, vector<MSG_CONFIG>& vMsgXmlNode)
{
	if (NULL == pNode)
	{
		return false;
	}
	
	TiXmlElement* pChild = pNode->FirstChildElement();
    for (; pChild != NULL; pChild = pChild->NextSiblingElement())
    {
        MSG_CONFIG node;
		
		int value = 0;
		pChild->Attribute("id", &value);
		node.id = value;
		
		pChild->Attribute("size", &value);
		node.size = value;
		
		pChild->Attribute("cnt", &value);
		node.cnt = value;
		
		pChild->Attribute("offset", &value);
		node.offset = value;
		
		pChild->Attribute("imu", &value);
		node.imu = value;

        vMsgXmlNode.push_back(node);
    }

	return true;
}

bool CXml::PaserOptionTagNode(TiXmlElement* pNode, vector<OPTION>& vOption)
{
	if (NULL == pNode)
	{
		return false;
	}
	
	TiXmlElement* pChild = pNode->FirstChildElement();
	for (; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		OPTION op;
		
		op.key = pChild->Attribute("key");
		
		pChild->Attribute("value", &op.value);
		
		vOption.push_back(op);
	}
	
	return true;
}

