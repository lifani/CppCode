#include <xml/xml.h>

bool CXml::ReadXml(const string& strPath, vector<xml_node>& vXmlNode)
{
	vXmlNode.clear();
	
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
        xml_node xmlNode;

        if (!PaserTagNode(pChild, xmlNode))
        {
            return false;
        }

        vXmlNode.push_back(xmlNode);
    }

	
	return true;
}

bool CXml::PaserTagNode(TiXmlElement* pNode, xml_node& xmlNode)
{
	if (NULL == pNode)
    {
        return false;
    }

	// name
	xmlNode.name = pNode->Attribute("name");
	
	vector<xml_node>& vXmlNode = xmlNode.vXmlNode;
	
    // Read node
    TiXmlElement* pChild = pNode->FirstChildElement();
    for (; pChild != NULL; pChild = pChild->NextSiblingElement())
    {
        xml_node node;
		node.name = pChild->Attribute("name");

        vXmlNode.push_back(node);
    }

    return true;
}

