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

    // ��ȡ������
    bool ReadTreeXml(string strIn, vector<TestElement>& vTestElement);

    // ���������
    bool WriteTreeXml(string strIn, vector<TestElement>& vTestElement);

    // ��ȡ�ļ�·��
    bool ReadPathXml(string strIn, vector<string>& vPath);

    // �����ļ�·��
    bool WritePathXml(string strIn, vector<string>& vPath);

    // �������ļ�
    bool ReadArgXml(string strIn, vector<string>& argv);

    // д�����ļ�
    bool WriteArgXml(string strIn, vector<string>& argv);

private:

    bool PaserTagNode(TiXmlElement* pNode, TestElement& tElement);

    bool ReadXml(string strIn, const string& strAttribut, vector<string>& vOut);

    bool WriteXml(string strIn, vector<string>& vIn, const string& strRootTag, 
        const string& strTag, const string& strAttribute);

};

#endif
