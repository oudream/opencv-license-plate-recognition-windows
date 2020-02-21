#include "platelocate.h"

PlateLocate::PlateLocate()
{
    m_plateDetect = new CPlateDetect();
}

PlateLocate::~PlateLocate()
{
    SAFE_RELEASE(m_plateDetect);
}

int PlateLocate::start(const cv::Mat &src, std::vector<cv::Mat> &resultVec)
{
    std::vector<CPlate> plateVec;
    int resultPD = m_plateDetect->plateDetect(src, plateVec);

    if (resultPD == 0) {
        size_t num = plateVec.size();

        if (num > 0) {
            for (size_t i = 0; i < num; i++) {
                CPlate item = plateVec[i];
                cv::Mat plate = item.getPlateMat();
                resultVec.push_back(plate);
            }

            return 0;
        }
    }

    return -1;
}
