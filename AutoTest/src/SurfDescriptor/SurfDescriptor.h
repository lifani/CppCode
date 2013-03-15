#ifndef _SURF_DESCRIPTOR_H_
#define _SURF_DESCRIPTOR_H_

#include "../common/DllInterface.h"

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/legacy.hpp>

#include "../xml/xml.h"

using namespace cv;

class CSurfDescriptor;

typedef void (CSurfDescriptor::* extratorComputeFunc)(const Mat& img, vector<KeyPoint>& keyPoints, Mat& descriptors);
typedef void (CSurfDescriptor::* matchFunc)(const Mat& descriptor1, const Mat& descriptor2, vector<DMatch>& vDMatch);

class CSurfDescriptor : public CDllInterface
{
public:
    CSurfDescriptor(void);
    virtual ~CSurfDescriptor(void);

    int StartTest(const string& strIn, const string& strOut, const string& strParaAdditional);

    void SurfExtrator(const Mat& img, vector<KeyPoint>& keyPoints, Mat& descriptors);

    void FlannMatch(const Mat& descriptor1, const Mat& descriptor2, vector<DMatch>& vDMatch);

private:

    bool FileDMatchOut(FILE* pf, vector<DMatch>& vDMatch);

private:

    CXml m_CXml;
};

#endif