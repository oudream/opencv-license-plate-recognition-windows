#ifndef CHARSCUTTING_H
#define CHARSCUTTING_H

#include "common.h"
#include "imageadjust.h"

class CharsCutting
{
public:
    CharsCutting();
    ~CharsCutting();

    inline void setCuttingMode(const int& mode) {m_cuttingMode = mode;}

    int start(const cv::Mat& src, std::vector<cv::Mat>& charsVec);

    static cv::Rect GetCenterRect(const cv::Mat &in);

    static Mat CutTheRect(const Mat &in, Rect &rect);

private:
    int cutByProjection(const cv::Mat& src, std::vector<cv::Mat>& charsVec);
    int cutByContourBlock(const cv::Mat& src, std::vector<cv::Mat>& charsVec);


    int splitChar(const cv::Mat& src, std::vector<cv::Mat>& out);

    int getAllSplitArea(const cv::Mat& src, const int& charWidth, std::vector<cv::Rect>& allSplitRect);

    bool pixCompensation(cv::Mat& src,const int& type,const int& count);

    bool CheckCharSizes(cv::Mat r);

    int getCharBorder(const std::vector<cv::Rect>& vecRect, const int& matWidth, std::map<std::string,int>& charBorder);

    void resizeChar(const cv::Mat& in, cv::Mat& out);


private:
    int m_cuttingMode;

};

#endif // CHARSCUTTING_H
