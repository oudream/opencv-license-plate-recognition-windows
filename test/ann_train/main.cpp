#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "easypr/config.h"
#include "easypr/util/util.h"


#include <iostream>
#include <string>

using namespace std;
using namespace easypr;

cv::Ptr<cv::ml::TrainData> trainData();
void test();
cv::Mat features(const cv::Mat& in);
cv::Rect GetCenterRect(const cv::Mat &in);
cv::Mat CutTheRect(const cv::Mat &in, cv::Rect &rect);
cv::Mat getHistogramFeature(const cv::Mat& in,const cv::Rect& rect);

int main(int argc, char *argv[])
{
    cv::Ptr<cv::ml::ANN_MLP> ann = cv::ml::ANN_MLP::create();

    cv::Mat layerSizes(1, 3, CV_32SC1);
    layerSizes.at<int>(0) = 200;                // 输入层节点数
    layerSizes.at<int>(1) = 40;                 // 隐藏层节点数
    layerSizes.at<int>(2) = 34;                 // 输出层节点数

    ann->setLayerSizes(layerSizes);
    ann->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
    ann->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER+cv::TermCriteria::EPS, 2000, 0.0000001));
    ann->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::BACKPROP);
    ann->setBackpropWeightScale(0.1);
    ann->setBackpropMomentumScale(0.1);


    cv::Ptr<cv::ml::TrainData> tData = trainData();
    ann->train(tData);
    ann->save("ann.xml");

    test();

//    float response = ann->predict(testMat);

    cv::waitKey(0);
    return 0;
}

cv::Ptr<cv::ml::TrainData> trainData()
{

    cv::Mat samples;
    std::vector<int> labels;


    for (int i = 0; i < 34; ++i) {
        auto char_key = kChars[i];
        char sub_folder[512] = {0};

        sprintf(sub_folder, "%s/%s", "train/eng", char_key);
        std::cout << "  >> Featuring characters " << char_key << " in "
                  << sub_folder << std::endl;

        auto chars_files = utils::getFiles(sub_folder);
        for (auto file : chars_files) {
            auto img = cv::imread(file,cv::IMREAD_GRAYSCALE);  // a grayscale image
            auto fps = features(img);

            samples.push_back(fps);
            labels.push_back(i);
        }
    }

    cv::Mat samples_;
    samples.convertTo(samples_, CV_32F);
    cv::Mat train_classes = cv::Mat::zeros((int)labels.size(), 34, CV_32F);

    for (int i = 0; i < train_classes.rows; ++i) {
        train_classes.at<float>(i, labels[i]) = 1.f;
    }

    return cv::ml::TrainData::create(samples_, cv::ml::SampleTypes::ROW_SAMPLE,train_classes);
}

void test()
{
    cv::Ptr<cv::ml::ANN_MLP> _ann;
    _ann = cv::ml::ANN_MLP::load<cv::ml::ANN_MLP>("ann.xml");

    auto chars_files = utils::getFiles("test");
    for (auto file : chars_files) {
        auto img = cv::imread(file,cv::IMREAD_GRAYSCALE);  // a grayscale image
        auto fps = features(img);

        float response = _ann->predict(fps);
        int index = static_cast<int>(response);
        cout << response << " : " << index << ", result:" << kChars[index] << endl;

    }

}




cv::Mat features(const cv::Mat& in)
{

    //抠取中间区域

    cv::Rect _rect = GetCenterRect(in);

    cv::Mat tmpIn = CutTheRect(in, _rect);

    // Last 10 is the number of moments components
//    cv::threshold(tmpIn,tmpIn, 150, 255, CV_THRESH_OTSU);

//    int tmpW = tmpIn.cols;
//    int tmpH = tmpIn.rows;

//    std::vector<cv::Mat> histVec;

//    for (int m = 0; m < 4; m++) {

//        cv::Rect partRect;

//        switch (m) {
//        case 0:
//        {
//            partRect = cv::Rect(0,0,tmpW/2,tmpH/2);
//        }
//            break;
//        case 1:
//        {
//            partRect = cv::Rect(tmpW/2,0,tmpW/2,tmpH/2);
//        }
//            break;
//        case 2:
//        {
//            partRect = cv::Rect(0,tmpH/2,tmpW/2,tmpH/2);
//        }
//            break;
//        case 3:
//        {
//            partRect = cv::Rect(tmpW/2,tmpH/2,tmpW/2,tmpH/2);
//        }
//            break;
//        default:
//            break;
//        }

//        cv::Mat histMat = getHistogramFeature(tmpIn,partRect);

//        histVec.push_back(histMat);
//    }


    int numCols = tmpIn.rows * tmpIn.cols;
//    int numCols = tmpIn.rows * tmpIn.cols + (histVec[0].cols * 4);

    cv::Mat out = cv::Mat::zeros(1, numCols, CV_32F);

    int n = 0;
    for (int i = 0; i < tmpIn.rows; i++) {
        for (int j = 0; j < tmpIn.cols; j++) {
            out.at<float>(n) = tmpIn.at<uchar>(i,j);
            n++;
        }
    }

//    for (int m = 0; m < 4; m++) {
//        for (int j = 0; j < histVec[m].cols; j++) {
//            out.at<float>(n) = histVec[m].at<uchar>(j);
//            n++;
//        }
//    }


    return out;
}

cv::Rect GetCenterRect(const cv::Mat &in)
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
  for (int i = in.rows - 1;
  i >= 0;
  --i) {
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
  for (int j = in.cols - 1;
  j >= 0;
  --j) {
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


cv::Mat CutTheRect(const cv::Mat &in, cv::Rect &rect)
{

    int h = 20;
    int w = 10;

    cv::Mat tmpIn = cv::Mat(in,rect);

    cv::Mat dstMat(h, w, tmpIn.type());
    dstMat.setTo(cv::Scalar(0));

    if (tmpIn.cols >= dstMat.cols) {
        cv::resize(tmpIn,dstMat,cv::Size(w, h));
    }
    else{
        int x = (int)((dstMat.cols - tmpIn.cols) / 2.0f);
        int y = (int)((dstMat.rows - tmpIn.rows) / 2.0f);

        //把截取的图形，拷到目标mat的中间

        for (int i = 0; i < tmpIn.rows; i++) {

            uchar* inData = tmpIn.ptr<uchar>(i);
            uchar* dstData = dstMat.ptr<uchar>(y + i);

            for (int j = 0; j < tmpIn.cols; j++) {
                dstData[x + j] = inData[j];
            }
        }
    }

    cv::imshow("test",dstMat);
    //

    return dstMat;
}

cv::Mat getHistogramFeature(const cv::Mat& in,const cv::Rect& rect)
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
