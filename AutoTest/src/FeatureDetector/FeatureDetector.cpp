#include "FeatureDetector.h"

CDllInterface* g_pDll = NULL;

CDllInterface* CreateInstance()
{
    if (NULL == g_pDll)
    {
        g_pDll = new CFeatureDetector;
    }

    return g_pDll;
}
// pf是可用文件句柄
bool FileKeyPointsOut(FILE* pf, vector<KeyPoint>& vKeyPoint)
{
    if (NULL == pf)
    {
        return false;
    }

    fprintf(pf, "%d", vKeyPoint.size());

    vector<KeyPoint>::iterator itr = vKeyPoint.begin();
    for (; itr != vKeyPoint.end(); ++itr)
    {
        fprintf(pf, "\n%f\t%f\t%f\t%f\t%d\t%d\t%0.14f",
            itr->pt.x, itr->pt.y, itr->size, itr->angle, itr->octave, itr->class_id, itr->response);
    }

    fprintf(pf, "\n");

    return true;
}

bool FileKeyPointsIn(FILE* pf, long& pos, vector<KeyPoint>& vKeyPoint)
{
    if (pf == NULL || fseek(pf, pos, SEEK_SET) != 0)
    {
        return false;
    }

    int cnt = 0;
    fscanf_s(pf, "%d", &cnt);

    int i = 0;
    while (!feof(pf) && i < cnt)
    {
        KeyPoint p;
        fscanf_s(pf, "%f %f %f %f %d %d %0.14f", 
            &p.pt.x, &p.pt.y, &p.size, &p.angle, &p.octave, &p.class_id, &p.response);

        vKeyPoint.push_back(p);

        i++;
    }

    pos = ftell(pf);

    return true;
}

CFeatureDetector::CFeatureDetector(void) 
{
}

CFeatureDetector::~CFeatureDetector(void)
{
}

int CFeatureDetector::StartTest( const string& strIn, const string& strOut, const string& strParaAdditional )
{
    FILE* pf = fopen(strOut.c_str(), "w+");
    if (NULL == pf)
    {
        return -1;
    }

    int nRet = ProcessVideo(strIn, pf);

    fclose(pf);
    pf = NULL;

    return nRet;
}

int CFeatureDetector::ProcessVideo( const string& strIn, FILE* pf )
{
    VideoCapture vc(strIn);
    if (!vc.isOpened())
    {
        return -1;
    }

    FastFeatureDetector fast(40);

    int tt = 0;
    int cnt = 0;
    Mat img;
    while (vc.read(img))
    {
        vector<KeyPoint> vKeyPoint;

        double t = getTickCount();
        fast.detect(img, vKeyPoint);
        t = (getTickCount() - t) * 1000 / getTickFrequency();

        FileKeyPointsOut(pf, vKeyPoint);
        fflush(pf);
    
        tt += (int)t;
        ++cnt;
    }

    vc.release();

    return tt / cnt;
}
