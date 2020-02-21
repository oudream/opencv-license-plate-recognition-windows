#include "cxann.h"

static fn_outInfo_t fn_outInfo_ann = 0;

void outInfo_ann(const char * sInfo)
{
    if (fn_outInfo_ann) fn_outInfo_ann(sInfo);
}

CxAnn::CxAnn(const int& charType)
    : m_charType(charType)
{
    ann = cv::ml::ANN_MLP::create();
}

CxAnn::~CxAnn()
{

}

void CxAnn::dataTrain()
{
    int inputCount = 0;
    int annCount = 0;
    int outputCount = 0;
    size_t iterations = 0;
    double epsilon = 0.0;

    std::string modelName;
    const char** charSet;

    cv::Mat layerSizes(1, 3, CV_32SC1);

    switch (m_charType) {
    case AnnCharType::ENG_CHAR:
    {
        annCount = 50;
        outputCount = engCharCount;
        iterations = 10000;
        epsilon = 0.000001;
        modelName = annEngModelPath;
        charSet = engChars;
    }
        break;
    case AnnCharType::CHI_CHAR:
    {
        annCount = 40;
        outputCount = chiCharCount;
        iterations = 2000;
        epsilon = 0.000001;
        modelName = annChiModelPath;
        charSet = chiChars;
    }
    default:
        break;
    }

    cv::Ptr<cv::ml::TrainData> trainData = getTrainData(outputCount,charSet,inputCount);
    std::cout << inputCount << std::endl;

    layerSizes.at<int>(0) = inputCount;                // 输入层节点数
    layerSizes.at<int>(1) = annCount;                  // 隐藏层节点数
    layerSizes.at<int>(2) = outputCount;               // 输出层节点数

    ann->setLayerSizes(layerSizes);
    ann->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
    ann->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER+cv::TermCriteria::EPS, iterations, epsilon));
    ann->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::BACKPROP);
    ann->setBackpropWeightScale(0.1);
    ann->setBackpropMomentumScale(0.1);

    ann->train(trainData);
    ann->save(modelName);

}

std::string CxAnn::dataPredict(const cv::Mat &inMat, const int& charType)
{
    std::string model;
    const char** charSet;

    if (charType == AnnCharType::ENG_CHAR) {
        model = std::string(annEngModelPath);
        charSet = engChars;
    }
    else if (charType == AnnCharType::CHI_CHAR) {
        model = std::string(annChiModelPath);
        charSet = chiChars;
    }

    cv::Ptr<cv::ml::ANN_MLP> _ann;
    _ann = cv::ml::ANN_MLP::load<cv::ml::ANN_MLP>(model);

    auto featMat = CharFeature::getFeatures(inMat,FeatureMethod::HOG);

    float response = _ann->predict(featMat);
    int index = static_cast<int>(response);

    return std::string(charSet[index]);
}

cv::Ptr<ml::TrainData> CxAnn::getTrainData(const int& charCount, const char** charSet, int& dimensionCount)
{
    cv::Mat samples;
    std::vector<int> labels;

    for (int i = 0; i < charCount; ++i) {
        auto m_char = charSet[i];
        char subFolder[512] = {0};

        if (m_charType == AnnCharType::ENG_CHAR) {
            sprintf(subFolder, "%s/%s", annEngTrainPath, m_char);
        }
        else if (m_charType == AnnCharType::CHI_CHAR) {
            sprintf(subFolder, "%s/%d", annChiTrainPath, i);
        }

        std::stringstream ss;
        ss << "  >> Get train characters " << m_char << " in " << subFolder;
        std::string s = ss.str();
        Common::outInfo(s.c_str());

        auto charsFiles = utils::getFiles(subFolder);
        for (auto file : charsFiles) {
            cv::Mat img = cv::imread(file,cv::IMREAD_GRAYSCALE);  // a grayscale image
            cv::Mat feat = CharFeature::getFeatures(img,FeatureMethod::HOG);

            samples.push_back(feat);
            labels.push_back(i);
        }
    }

    dimensionCount = samples.cols;

    cv::Mat samples_;
    samples.convertTo(samples_, CV_32F);

    cv::Mat train_classes = cv::Mat::zeros((int)labels.size(), charCount, CV_32F);

    for (int i = 0; i < train_classes.rows; ++i) {
        train_classes.at<float>(i, labels[i]) = 1.f;
    }

    return cv::ml::TrainData::create(samples_, cv::ml::SampleTypes::ROW_SAMPLE,train_classes);
}
