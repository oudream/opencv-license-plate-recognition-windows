#ifndef CXSVM_H
#define CXSVM_H

#include "common.h"
#include "charfeature.h"

class CxSvm
{
public:
    CxSvm(const int& charType);
    ~CxSvm();

    void dataTrain();
    static std::string dataPredict(const cv::Mat& inMat, const int &charType);

private:
    cv::Ptr<cv::ml::TrainData> getTrainData(const int& charCount, const char **charSet);

private:
    cv::Ptr<cv::ml::SVM> svm;
    int m_charType;

};

#endif // CXSVM_H
