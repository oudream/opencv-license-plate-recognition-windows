#include "charfeature.h"

CharFeature::CharFeature()
{

}

cv::Mat CharFeature::getFeatures(const cv::Mat& in, const int& featureMethod)
{
    switch (featureMethod) {
    case FeatureMethod::PIXELS :
        return getPixelsFeature(in);
        break;

    case FeatureMethod::LBP :
        return getLbpFeature(in,1,8);
        break;

    case FeatureMethod::HOG :
        return getHogFeature(in);
        break;

    default:
        break;
    }
}

//cv::Mat CharFeature::getFeaturesForEngChar(const cv::Mat &inMat)
//{
//    cv::Mat tmpIn = inMat.clone();

//    int tmpW = tmpIn.cols;
//    int tmpH = tmpIn.rows;

////    std::vector<cv::Mat> histVec;

////    // 把字符分成4个区域，分别求水平竖直直方图作为辅助特征
////    for (int m = 0; m < 4; m++) {

////        cv::Rect partRect;

////        switch (m) {
////        case 0:
////        {
////            partRect = cv::Rect(0,0,tmpW/2,tmpH/2);
////        }
////            break;
////        case 1:
////        {
////            partRect = cv::Rect(tmpW/2,0,tmpW/2,tmpH/2);
////        }
////            break;
////        case 2:
////        {
////            partRect = cv::Rect(0,tmpH/2,tmpW/2,tmpH/2);
////        }
////            break;
////        case 3:
////        {
////            partRect = cv::Rect(tmpW/2,tmpH/2,tmpW/2,tmpH/2);
////        }
////            break;
////        default:
////            break;
////        }

////        // 求水平竖直直方图
////        cv::Mat histMat = getHistogramFeature(tmpIn,partRect);

////        histVec.push_back(histMat);
////    }

//    // 把多维矩阵变成一维数组
////    cv::threshold(tmpIn,tmpIn, 150, 255, CV_THRESH_OTSU);
//    int numCols = tmpW * tmpH;
////    int numCols = tmpIn.rows * tmpIn.cols + (histVec[0].cols * 4);

//    // 创建一维的MAT矩阵
//    cv::Mat outMat = cv::Mat::zeros(1, numCols, CV_32F);

//    // 把原来多维矩阵数据一一对应地拷到一维矩阵中，形成特征向量矩阵
//    int n = 0;
//    for (int i = 0; i < tmpH; i++) {
//        for (int j = 0; j < tmpW; j++) {
//            outMat.at<float>(n) = tmpIn.at<uchar>(i,j);
//            n++;
//        }
//    }

////    for (int m = 0; m < 4; m++) {
////        for (int j = 0; j < histVec[m].cols; j++) {
////            outMat.at<float>(n) = histVec[m].at<uchar>(j);
////            n++;
////        }
////    }

//    return outMat;
//}


cv::Rect CharFeature::GetCenterRect(const cv::Mat &in)
{
    cv::Rect _rect;

    int top = 0;
    int bottom = in.rows - 1;

    //上下

    for (int i = 0; i < in.rows; ++i) {
        bool bFind = false;
        for (int j = 0; j < in.cols; ++j) {
            if (in.data[i * in.step[0] + j] > 20) {
                top = i;
                bFind = true;
                break;
            }
        }
        if (bFind) {
            break;
        }

        //统计这一行或一列中，非零元素的个数

    }
    for (int i = in.rows - 1; i >= 0; --i) {
        bool bFind = false;
        for (int j = 0; j < in.cols; ++j) {
            if (in.data[i * in.step[0] + j] > 20) {
                bottom = i;
                bFind = true;
                break;
            }
        }
        if (bFind) {
            break;
        }

        //统计这一行或一列中，非零元素的个数

    }

    //左右

    int left = 0;
    int right = in.cols - 1;
    for (int j = 0; j < in.cols; ++j) {
        bool bFind = false;
        for (int i = 0; i < in.rows; ++i) {
            if (in.data[i * in.step[0] + j] > 20) {
                left = j;
                bFind = true;
                break;
            }
        }
        if (bFind) {
            break;
        }

        //统计这一行或一列中，非零元素的个数

    }
    for (int j = in.cols - 1; j >= 0; --j) {
        bool bFind = false;
        for (int i = 0; i < in.rows; ++i) {
            if (in.data[i * in.step[0] + j] > 20) {
                right = j;
                bFind = true;

                break;
            }
        }
        if (bFind) {
            break;
        }

        //统计这一行或一列中，非零元素的个数

    }

    _rect.x = left;
    _rect.y = top;
    _rect.width = right - left + 1;
    _rect.height = bottom - top + 1;

    return _rect;
}

cv::Mat CharFeature::CutTheRect(const cv::Mat &in, cv::Rect &rect)
{
    int h = 20;
    int w = 10;

    cv::Mat tmpIn = cv::Mat(in,rect);

    cv::Mat destMat(h, w, tmpIn.type());
    destMat.setTo(cv::Scalar(0));

    if (tmpIn.cols >= destMat.cols) {
        cv::resize(tmpIn,destMat,cv::Size(w, h));
    }
    else{
        int x = (int)((destMat.cols - tmpIn.cols) / 2.0f);
        int y = (int)((destMat.rows - tmpIn.rows) / 2.0f);

        //把截取的图形，拷到目标mat的中间

        for (int i = 0; i < tmpIn.rows; i++) {

            uchar* inData = tmpIn.ptr<uchar>(i);
            uchar* destData = destMat.ptr<uchar>(y + i);

            for (int j = 0; j < tmpIn.cols; j++) {
                destData[x + j] = inData[j];
            }
        }
    }

    return destMat;
}

cv::Mat CharFeature::getPixelsFeature(const cv::Mat &in)
{
    //抠取中间区域

    cv::Rect _rect = CharFeature::GetCenterRect(in);

    cv::Mat tmpIn = CharFeature::CutTheRect(in, _rect);


    int tmpW = tmpIn.cols;
    int tmpH = tmpIn.rows;

    // 把多维矩阵变成一维数组
    int numCols = tmpW * tmpH;

    // 创建一维的MAT矩阵
    cv::Mat out = cv::Mat::zeros(1, numCols, CV_32F);

    // 把原来多维矩阵数据一一对应地拷到一维矩阵中，形成特征向量矩阵
    int n = 0;
    for (int i = 0; i < tmpH; i++) {
        for (int j = 0; j < tmpW; j++) {
            out.at<float>(n) = tmpIn.at<uchar>(i,j);
            n++;
        }
    }

    return out;
}

/* 暂时没用 */
cv::Mat CharFeature::getHistogramFeature(const cv::Mat &in, const cv::Rect &rect)
{
    cv::Mat tmpIn = cv::Mat(in,rect);

    int w = tmpIn.cols;
    int h = tmpIn.rows;

    cv::Mat allHist = cv::Mat::zeros(1,h + w,tmpIn.type());

    //竖直方向直方图
    cv::Mat vHist = cv::Mat::zeros(1,h,tmpIn.type());

    for (int i = 0; i < h; i++) {
        int noneBlackCount = 0;
        for (int j = 0; j < w; j++) {
            if (tmpIn.at<uchar>(j,i) != 0) {
                noneBlackCount++;
            }
        }

        vHist.at<uchar>(i) = noneBlackCount;
        allHist.at<uchar>(i) = noneBlackCount;
    }

    //水平方向直方图
    cv::Mat hHist = cv::Mat::zeros(1,w,tmpIn.type());

    for (int i = 0; i < w; i++) {
        int noneBlackCount = 0;
        for (int j = 0; j < h; j++) {
            if (tmpIn.at<uchar>(i,j) != 0) {
                noneBlackCount++;
            }
        }

        hHist.at<uchar>(i) = noneBlackCount;
        allHist.at<uchar>(i + h) = noneBlackCount;
    }

    return allHist;
}

cv::Mat CharFeature::getLbpFeature(const cv::Mat &in, const int &radius, const int &neighbors)
{
    cv::Mat dst = cv::Mat(in.rows-2*radius, in.cols-2*radius,CV_8UC1, cv::Scalar(0));

    for (int n=0; n<neighbors; n++) {
        // 采样点的计算
        float x = static_cast<float>(-radius * sin(2.0*CV_PI*n/static_cast<float>(neighbors)));
        float y = static_cast<float>(radius * cos(2.0*CV_PI*n/static_cast<float>(neighbors)));

        // 上取整和下取整的值
        int fx = static_cast<int>(floor(x));
        int fy = static_cast<int>(floor(y));
        int cx = static_cast<int>(ceil(x));
        int cy = static_cast<int>(ceil(y));

        // 小数部分
        float ty = y - fy;
        float tx = x - fx;

        // 设置插值权重
        float w1 = (1 - tx) * (1 - ty);
        float w2 =      tx  * (1 - ty);
        float w3 = (1 - tx) *      ty;
        float w4 =      tx  *      ty;

        // 循环处理图像数据
        for(int i=radius; i < in.rows-radius;i++)
        {
            for(int j=radius;j < in.cols-radius;j++)
            {
                // 计算插值
                float t = static_cast<float>(w1*in.at<uchar>(i+fy,j+fx) + w2*in.at<uchar>(i+fy,j+cx) + w3*in.at<uchar>(i+cy,j+fx) + w4*in.at<uchar>(i+cy,j+cx));
                // 进行编码
                dst.at<uchar>(i-radius,j-radius) += ((t > in.at<uchar>(i,j)) || (std::abs(t-in.at<uchar>(i,j)) < std::numeric_limits<float>::epsilon())) << n;
            }
        }
    }

    return dst;
}

cv::Mat CharFeature::getHogFeature(const cv::Mat &in)
{
    // 创建存放HOG特征值的数组
    std::vector<float> descriptors;

    // 创建HOG特征获取器
    cv::HOGDescriptor* hog = new cv::HOGDescriptor(CvSize(20,20),CvSize(10,10),CvSize(5,5),CvSize(5,5),9);

    // 计算输入图像的HOG特征值，得到一个一维数组
    hog->compute(in,descriptors,CvSize(1,1),CvSize(0,0));

    // 创建一维的输出矩阵
    cv::Mat out = cv::Mat::zeros(1,descriptors.size(),CV_32FC1);

    // 把数组存放的特征值一一对应地写入输出矩阵
    for (size_t i = 0; i < descriptors.size(); i++) {
        out.at<float>(0,i) = descriptors[i];
    }

    delete hog;
    hog = NULL;

    return out;
}
