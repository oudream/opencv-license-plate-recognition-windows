#ifndef CXANN_H
#define CXANN_H

#include "common.h"
#include "charfeature.h"


class CxAnn
{
public:
    CxAnn(const int& charType);
    ~CxAnn();

    void dataTrain();
    static std::string dataPredict(const cv::Mat& inMat, const int &charType);

private:
    cv::Ptr<cv::ml::TrainData> getTrainData(const int& charCount, const char **charSet, int& dimensionCount);

private:
    cv::Ptr<cv::ml::ANN_MLP> ann;
    int m_charType;

};

#endif // CXANN_H
