#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include "charscutting.h"

#include <iostream>
#include <string>

using namespace std;

cv::Ptr<cv::ml::TrainData> trainData();
cv::Mat features(const cv::Mat& in);

int main()
{
    cv::Ptr<cv::ml::ANN_MLP> ann = cv::ml::ANN_MLP::create();

    cv::Mat layerSizes(1, 3, CV_32SC1);
    layerSizes.at<int>(0) = 200;                // the input layer
    layerSizes.at<int>(1) = 40;           // the neurons
    layerSizes.at<int>(2) = 34;  // the output layer

    ann->setLayerSizes(layerSizes);
    ann->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
//    ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS, 300, FLT_EPSILON));
    ann->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::BACKPROP);
    ann->setBackpropWeightScale(0.1);
    ann->setBackpropMomentumScale(0.1);


    cv::Ptr<cv::ml::TrainData> tData = trainData();
    ann->train(tData);
    ann->save("ann.xml");



//    float response = ann->predict(testMat);


    return 0;
}

cv::Ptr<cv::ml::TrainData> trainData()
{

    cv::Mat samples;
    std::vector<int> labels;


    for (int i = 0; i < 34; ++i) {
        auto char_key = kChars[i];
        char sub_folder[512] = {0};

        sprintf(sub_folder, "%s/%s", "train", char_key);
        std::cout << "  >> Featuring characters " << char_key << " in "
                  << sub_folder << std::endl;

        auto chars_files = utils::getFiles(sub_folder);
        for (auto file : chars_files) {
            auto img = cv::imread(file, 0);  // a grayscale image
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

cv::Mat features(const cv::Mat& in)
{

    //抠取中间区域

    cv::Rect _rect = CharsCutting::GetCenterRect(in);

    cv::Mat tmpIn = CharsCutting::CutTheRect(in, _rect);

    // Last 10 is the number of moments components
    int numCols = tmpIn.rows * tmpIn.cols;

    cv::Mat out = cv::Mat::zeros(1, numCols, CV_32F);
    // Asign values to

    // feature,ANN的样本特征为水平、垂直直方图和低分辨率图像所组成的矢量

    int n = 0;
    for (int i = 0; i < tmpIn.rows; i++) {
        for (int j = 0; j < tmpIn.cols; j++) {
            out.at<float>(n) = tmpIn.at<uchar>(i,j);
            n++;
        }
    }

    return out;
}
