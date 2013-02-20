#include <iostream>
#include "../xml/xml.h"

using namespace std;

int main(int argc, char* argv[])
{
    CXml* pXml = new CXml;
    vector<TestElement> vTestElement;

    pXml->ReadTreeXml("E:\\Code\\AutoTest\\dll.xml", vTestElement);

    pXml->WriteTreeXml("E:\\Code\\AutoTest\\dll01.xml", vTestElement);

    vector<string> vPath;
    pXml->ReadPathXml("E:\\Code\\AutoTest\\path.xml", vPath);
    pXml->WritePathXml("E:\\Code\\AutoTest\\path01.xml", vPath);

    return 0;
}