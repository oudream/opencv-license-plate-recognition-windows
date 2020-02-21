#ifndef CARNUMRECOGNIZE_H
#define CARNUMRECOGNIZE_H

#include "platelocate.h"
#include "charscutting.h"
#include "charsrecognize.h"

#include "imageadjust.h"


class CarNumRecognize
{
public:
    CarNumRecognize();
    ~CarNumRecognize();

    static int plateLocate(const cv::Mat& src, std::vector<cv::Mat>& resultVec,bool pdMode);

    static int charsCutting(const cv::Mat& src, std::vector<cv::Mat>& charsVec, const int& mode);

    static int charsRecognize(const std::vector<cv::Mat>& charsVec, std::vector<std::string>& resultVec,const int& mode,const char* lang = "");

    static int plateRecognize(const std::string& image, std::string& sResult, const int &encode, const int& cuttingMode = CuttingMode::CONTOUR_BLOCK, const int& recognizeMode = RecognizeMode::BOTH);

    static int getSingleCharPic(const std::string& image, const std::string &imageName);

private:
    static int getCorrectResult(const std::vector<std::vector<std::string> >& allResultVec, std::string& sResult);

};

#endif // CARNUMRECOGNIZE_H
