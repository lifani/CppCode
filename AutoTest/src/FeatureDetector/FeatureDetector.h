#ifndef _FEATURE_DETECTOR_H_
#define _FEATURE_DETECTOR_H_

#include "../common/DllInterface.h"

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

bool FileKeyPointsOut(FILE* pf, vector<KeyPoint>& vKeyPoint);

bool FileKeyPointsIn(FILE* pf, long pos, vector<KeyPoint>& vKeyPoint);

class CFeatureDetector : public CDllInterface
{
public:
    CFeatureDetector(void);
    virtual ~CFeatureDetector(void);

    int StartTest(const string& strIn, const string& strOut);

private:

    int ProcessVideo(const string& strIn, FILE* pf);

};

#endif
