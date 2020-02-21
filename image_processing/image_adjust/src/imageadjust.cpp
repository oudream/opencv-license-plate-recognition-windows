#include "imageadjust.h"

ImageAdjust::ImageAdjust()
{

}

ImageAdjust::~ImageAdjust()
{

}

int ImageAdjust::ContrastAdjust(const IplImage *srcImg, IplImage *dstImg, const float nPercent)
{
    for (int y = 0; y < srcImg->height; y++)
    {
        for (int x = 0; x < srcImg->width; x++)
        {
            float val = ((uchar*)(srcImg->imageData + srcImg->widthStep*y))[x];
            val = 128 + (val - 128) * nPercent;

            //对灰度值的可能溢出进行处理
            if (val > 255) val = 255;
            if (val < 0) val = 0;

            ((uchar*)(dstImg->imageData + dstImg->widthStep*y))[x] = (uchar)val;
        }
    }

    return 0;
}

int ImageAdjust::BrightnessAdjust(const IplImage *srcImg, IplImage *dstImg, float brightness)
{
    int x,y,i;
    float val;
    for (i = 0; i < 3; i++)//彩色图像需要处理3个通道
    {
        for (y = 0; y < srcImg->height; y++)
        {
            for (x = 0; x < srcImg->width; x++)
            {
                val = ((uchar*)(srcImg->imageData + srcImg->widthStep*y))[x*3+i];
                val += brightness;
                //对灰度值的可能溢出进行处理
                if(val>255) val=255;
                if(val<0) val=0;
                ((uchar*)(dstImg->imageData + dstImg->widthStep*y))[x*3+i] = (uchar)val;
            }
        }
    }

    return 0;
}

int ImageAdjust::EqualizeHistForImage(const cv::Mat &src, cv::Mat &dest, const int &channels)
{
    if (channels == ImageChannel::MULTI) {
        cv::Mat matHSV;
        cv::cvtColor(src,matHSV,CV_RGB2HSV);

        std::vector<cv::Mat> splitHSV;
        cv::split(matHSV,splitHSV);
        cv::equalizeHist(splitHSV[2],splitHSV[2]);
        cv::merge(splitHSV,matHSV);

        cv::cvtColor(matHSV,dest,CV_HSV2RGB);
    }
    else if (channels == ImageChannel::SINGLE) {
        cv::equalizeHist(src,dest);
    }

    return 0;
}
