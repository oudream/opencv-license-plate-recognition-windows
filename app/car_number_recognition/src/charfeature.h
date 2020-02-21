#ifndef CHARFEATURE_H
#define CHARFEATURE_H

#include "common.h"


class CharFeature
{
public:
    CharFeature();
    ~CharFeature();

    static cv::Mat getFeatures(const cv::Mat& in, const int& featureMethod);

private:

    static cv::Rect GetCenterRect(const cv::Mat &in);
    static cv::Mat CutTheRect(const cv::Mat &in, cv::Rect &rect);

    /* 获取特征的方法 */
    static cv::Mat getPixelsFeature(const cv::Mat& in);
    static cv::Mat getLbpFeature(const cv::Mat& in, const int& radius, const int& neighbors);
    static cv::Mat getHogFeature(const cv::Mat& in);

    /* 获取特征的辅助方法 */
    static cv::Mat getHistogramFeature(const cv::Mat& in,const cv::Rect& rect);

};

#endif // CHARFEATURE_H
