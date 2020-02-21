#ifndef IMAGEADJUST_H
#define IMAGEADJUST_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


namespace ImageChannel {
    const int SINGLE = 0;
    const int MULTI = 1;
}


class ImageAdjust
{
public:
    ImageAdjust();
    ~ImageAdjust();

    static int ContrastAdjust(const IplImage* srcImg, IplImage* dstImg, const float nPercent);

    static int BrightnessAdjust(const IplImage* srcImg, IplImage* dstImg, float brightness);

    static int EqualizeHistForImage(const cv::Mat &src, cv::Mat &dest, const int& channels);
};

#endif // IMAGEADJUST_H
