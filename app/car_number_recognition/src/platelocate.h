#ifndef PLATELOCATE_H
#define PLATELOCATE_H

#include "common.h"

class PlateLocate
{
public:
    PlateLocate();
    ~PlateLocate();

    inline void setPdMode(bool param){m_plateDetect->setPDLifemode(param);}

    int start(const cv::Mat &src, std::vector<cv::Mat> &resultVec);


private:
    CPlateDetect* m_plateDetect;

};

#endif // PLATELOCATE_H
