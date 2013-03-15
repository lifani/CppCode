// xml.h
// author   :       Li Fa Ni
// Date     :       2013-02-20
// Ver      :       1.0

#ifndef _XML_H_
#define _XML_H_

#include "../../third/tinyxml/tinyxml.h"
#include "../../third/tinyxml/tinystr.h"
#include "../../include/typedef.h"

class DLL_API CXml
{
public:
    CXml();

    ~CXml();

    // 读取对象树
    bool ReadTreeXml(string strIn, vector<TestElement>& vTestElement);

    // 保存对象树
    bool WriteTreeXml(string strIn, vector<TestElement>& vTestElement);

    // 读取文件路径
    bool ReadPathXml(string strIn, vector<string>& vPath);

    // 保存文件路径
    bool WritePathXml(string strIn, vector<string>& vPath);

    // 读参数文件
    bool ReadArgXml(string strIn, vector<string>& argv);

    // 写参数文件
    bool WriteArgXml(string strIn, vector<string>& argv);

private:

    bool PaserTagNode(TiXmlElement* pNode, TestElement& tElement);

    bool ReadXml(string strIn, const string& strAttribut, vector<string>& vOut);

    bool WriteXml(string strIn, vector<string>& vIn, const string& strRootTag, 
        const string& strTag, const string& strAttribute);

};

#endif
