#include "SurfDescriptor.h"
#include "../ToolBox/toolBox.h"

CDllInterface* g_pDll = NULL;

extratorComputeFunc g_extrator[] = {
    &CSurfDescriptor::SurfExtrator
};

matchFunc g_match[] = {
    &CSurfDescriptor::FlannMatch
};

CDllInterface* CreateInstance()
{
    if (NULL == g_pDll)
    {
        g_pDll = new CSurfDescriptor;
    }

    return g_pDll;
}

bool FileKeyPointsIn(FILE* pf, long& pos, vector<KeyPoint>& vKeyPoint)
{
    if (pf == NULL || fseek(pf, pos, SEEK_SET) != 0)
    {
        return false;
    }

    vKeyPoint.clear();

    int cnt = 0;
    fscanf_s(pf, "%d", &cnt);

    int i = 0;
    while (!feof(pf) && i < cnt)
    {
        KeyPoint p;
        fscanf_s(pf, "%f %f %f %f %d %d %f", 
            &p.pt.x, &p.pt.y, &p.size, &p.angle, &p.octave, &p.class_id, &p.response);

        vKeyPoint.push_back(p);

        i++;
    }

    pos = ftell(pf);

    return !feof(pf);
}

CSurfDescriptor::CSurfDescriptor(void)
{
}

CSurfDescriptor::~CSurfDescriptor(void)
{
}

int CSurfDescriptor::StartTest( const string& strIn, const string& strOut, const string& strParaAdditional )
{
    string strAppPath = GetAppPath();

    // 获取视频文件路径
    string strXml = strAppPath + SEPERATOR + EXECUTE_XML;
    vector<string> vPath;

    m_CXml.ReadPathXml(strXml, vPath);
    if (vPath.empty())
    {
        return -1;
    }

    string strVideoFile = vPath.at(0);
    VideoCapture vc(strVideoFile);
    if (!vc.isOpened())
    {
        return -1;
    }

    FILE* pf = fopen(strIn.c_str(), "r");
    if (NULL == pf)
    {
        vc.release();
        return -1;
    }

    FILE* pOf = fopen(strOut.c_str(), "w+");
    if (NULL == pOf)
    {
        vc.release();
        fclose(pf);
        pf = NULL;

        return -1;
    }

    int lenOfExtrator = sizeof(g_extrator) / sizeof(extratorComputeFunc);
    int lenOfMatch = sizeof(g_match) / sizeof(matchFunc);

    long pos = 0;
    vector<KeyPoint> vKeyPoint;

    Mat d1, d2;

    double t = getTickCount();

    Mat img;
    while (FileKeyPointsIn(pf, pos, vKeyPoint) && vc.read(img))
    {
        vector<DMatch> vDMatch;

        extratorComputeFunc func = g_extrator[0];
        (this->*g_extrator[0])(img, vKeyPoint, d2);
        if (!d1.empty())
        {
            (this->*g_match[0])(d1, d2, vDMatch);

            FileDMatchOut(pOf, vDMatch);
        }

        d1 = d2.clone();
    }

    fclose(pOf);

    t = (getTickCount() - t) * 1000 / getTickFrequency();

    fclose(pf);

    pOf = NULL;
    pf = NULL;

    vc.release();
    
    return t;
}

void CSurfDescriptor::SurfExtrator( const Mat& img, vector<KeyPoint>& keyPoints, Mat& descriptors )
{
    SurfDescriptorExtractor extrator;
    extrator.compute(img, keyPoints, descriptors);
}

void CSurfDescriptor::FlannMatch( const Mat& descriptor1, const Mat& descriptor2, vector<DMatch>& vDMatch )
{
    FlannBasedMatcher matcher;
    matcher.match(descriptor1, descriptor2, vDMatch);
}

bool CSurfDescriptor::FileDMatchOut( FILE* pf, vector<DMatch>& vDMatch )
{
    if (NULL == pf)
    {
        return false;
    }

    fprintf(pf, "%d\n", vDMatch.size());

    vector<DMatch>::iterator itr = vDMatch.begin();
    for (; itr != vDMatch.end(); ++itr)
    {
        fprintf(pf, "%d\t%d\t%d\t%f\n", itr->queryIdx, itr->trainIdx, itr->imgIdx, itr->distance);
    }

    return true;
}
