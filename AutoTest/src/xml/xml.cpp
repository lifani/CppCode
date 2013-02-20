// xml.cpp
// author   :       Li Fa Ni
// Date     :       2013-02-20
// Ver      :       1.0
#include "xml.h"

CXml::CXml()
{

}

CXml::~CXml()
{

}

bool CXml::ReadTreeXml(string strIn, vector<TestElement>& vTestElement)
{
    vTestElement.clear();

    // 创建xml对象
    TiXmlDocument tiXmlDoc(strIn.c_str());

    // load
    if (!tiXmlDoc.LoadFile())
    {
        return false;
    }

    TiXmlElement* pRoot = tiXmlDoc.RootElement();

    // Parse every tag
    TiXmlElement* pChild = pRoot->FirstChildElement();
    for (; pChild != NULL; pChild = pChild->NextSiblingElement())
    {
        TestElement tElement;

        if (!PaserTagNode(pChild, tElement))
        {
            return false;
        }

        vTestElement.push_back(tElement);
    }

    return true;
}

bool CXml::PaserTagNode( TiXmlElement* pNode, TestElement& tElement )
{
    if (NULL == pNode)
    {
        return false;
    }

    // id
    int id = 0; 
    pNode->Attribute(T_ID, &id);
    
    // desc
    string desc = pNode->Attribute(T_DESC);
    
    // attribute right?
    if (0 == id || desc.empty())
    {
        return false;
    }

    tElement.id = id;
    tElement.desc = desc;

    // Read tag
    TiXmlElement* pChild = pNode->FirstChildElement();
    for (; pChild != NULL; pChild = pChild->NextSiblingElement())
    {
        TestAtom tAtom;

        pChild->Attribute(T_ID, &tAtom.id);
        tAtom.name = pChild->Attribute(T_NAME);

        tElement.vTestAtom.push_back(tAtom);
    }

    return true;
}


bool CXml::WriteTreeXml( string strIn, vector<TestElement>& vTestElement )
{
    TiXmlDocument xmlDoc;;

    // Add root node
    TiXmlElement* pRoot = new TiXmlElement(N_ROOT);
    xmlDoc.LinkEndChild(pRoot);

    vector<TestElement>::iterator itr = vTestElement.begin();
    for (; itr != vTestElement.end(); ++itr)
    {
        // Add tags
        TiXmlElement* pElement = new TiXmlElement(N_TAGS);
        pRoot->LinkEndChild(pElement);

        pElement->SetAttribute(T_ID, itr->id);
        pElement->SetAttribute(T_DESC, itr->desc.c_str());

        vector<TestAtom>::iterator it = itr->vTestAtom.begin();
        for (; it != itr->vTestAtom.end(); ++it)
        {
            // Add tag
            TiXmlElement* pAtom = new TiXmlElement(N_TAG);
            pElement->LinkEndChild(pAtom);

            pAtom->SetAttribute(T_ID, it->id);
            pAtom->SetAttribute(T_NAME, it->name.c_str());
        }
    }

    xmlDoc.SaveFile(strIn.c_str());
    
    return true;
}


bool CXml::ReadPathXml( string strIn, vector<string>& vPath )
{
    vPath.clear();

    TiXmlDocument xmlDoc(strIn.c_str());
    if (!xmlDoc.LoadFile())
    {
        return false;
    }

    TiXmlElement* pElement = xmlDoc.RootElement();
    if (NULL == pElement)
    {
        return false;
    }

    // Read path
    TiXmlElement* pChild = pElement->FirstChildElement();
    for (; pChild != NULL; pChild = pChild->NextSiblingElement())
    {
        string strDesc = pChild->Attribute(T_DESC);

        if (!strDesc.empty())
        {
            vPath.push_back(strDesc);
        }
    }

    return true;
}

bool CXml::WritePathXml( string strIn, vector<string>& vPath )
{
    TiXmlDocument xmlDoc;

    TiXmlElement* pRoot = new TiXmlElement(N_PATHS);
    if (NULL == pRoot)
    {
        return false;
    }

    xmlDoc.LinkEndChild(pRoot);

    vector<string>::iterator itr = vPath.begin();
    for (; itr != vPath.end(); ++itr)
    {
        TiXmlElement* pNode = new TiXmlElement(N_PATH);
        pNode->SetAttribute(T_DESC, itr->c_str());

        pRoot->LinkEndChild(pNode);
    }

    xmlDoc.SaveFile(strIn.c_str());

    return true;
}

