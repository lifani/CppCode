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
   return ReadXml(strIn, T_DESC, vPath);
}

bool CXml::WritePathXml( string strIn, vector<string>& vPath )
{
    return WriteXml(strIn, vPath, N_PATHS, N_PATH, T_DESC);
}

bool CXml::ReadArgXml( string strIn, vector<string>& argv )
{
    return ReadXml(strIn, T_DESC, argv);
}

bool CXml::WriteArgXml( string strIn, vector<string>& argv )
{
    return WriteXml(strIn, argv, N_ARGS, N_ARG, T_DESC);
}

bool CXml::ReadXml( string strIn, const string& strAttribute, vector<string>& vOut )
{
    vOut.clear();

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

    // Read attribute
    TiXmlElement* pChild = pElement->FirstChildElement();
    for (; pChild != NULL; pChild = pChild->NextSiblingElement())
    {
        string strDesc = pChild->Attribute(strAttribute.c_str());

        if (!strDesc.empty())
        {
            vOut.push_back(strDesc);
        }
    }

    return true;
}

bool CXml::WriteXml( string strIn,
                    vector<string>& vIn, 
                    const string& strRootTag, 
                    const string& strTag, 
                    const string& strAttribute )
{

    TiXmlDocument xmlDoc;

    TiXmlElement* pRoot = new TiXmlElement(strRootTag.c_str());
    if (NULL == pRoot)
    {
        return false;
    }

    xmlDoc.LinkEndChild(pRoot);

    vector<string>::iterator itr = vIn.begin();
    for (; itr != vIn.end(); ++itr)
    {
        TiXmlElement* pNode = new TiXmlElement(strTag.c_str());
        pNode->SetAttribute(strAttribute.c_str(), itr->c_str());

        pRoot->LinkEndChild(pNode);
    }

    xmlDoc.SaveFile(strIn.c_str());

    return true;
}

