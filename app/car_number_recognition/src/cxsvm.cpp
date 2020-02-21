#include "cxsvm.h"


CxSvm::CxSvm(const int &charType)
    : m_charType(charType)
{
    svm = cv::ml::SVM::create();
}

CxSvm::~CxSvm()
{

}

void CxSvm::dataTrain()
{
    int outputCount = 0;
    size_t iterations = 0;
    double epsilon = 0.0;

    std::string modelName;
    const char** charSet;


    switch (m_charType) {
    case AnnCharType::ENG_CHAR:
    {
        outputCount = engCharCount;
        iterations = 100000;
        epsilon = 0.000001;
        modelName = svmEngModelPath;
        charSet = engChars;
    }
        break;
    case AnnCharType::CHI_CHAR:
    {
        outputCount = chiCharCount;
        iterations = 100000;
        epsilon = 0.000001;
        modelName = svmChiModelPath;
        charSet = chiChars;
    }
    default:
        break;
    }


    svm->setType(cv::ml::SVM::C_SVC);
    svm->setKernel(cv::ml::SVM::RBF);
     //svm->setDegree(10.0);
    svm->setGamma(0.01);
     //svm->setCoef0(1.0);
    svm->setC(10.0);
    //svm->setNu(0.5);
    //svm->setP(0.1);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER+cv::TermCriteria::EPS, iterations, epsilon));

    cv::Ptr<cv::ml::TrainData> trainData = getTrainData(outputCount,charSet);

    svm->trainAuto(trainData);
    std::cout << "type:" << svm->getType() << "; "
              << "kernel:" << svm->getKernelType() << "; "
              << "degree:" << svm->getDegree() << "; "
              << "gamma:" << svm->getGamma() << "; "
              << "Coef0:" << svm->getCoef0() << "; "
              << "C:" << svm->getC() << "; "
              << "Nu:" << svm->getNu() << "; "
              << "P:" << svm->getP() << "; " << std::endl;

//    svm->train(trainData);
    svm->save(modelName);
}

std::string CxSvm::dataPredict(const Mat &inMat, const int &charType)
{
    std::string model;
    const char** charSet;

    if (charType == AnnCharType::ENG_CHAR) {
        model = std::string(svmEngModelPath);
        charSet = engChars;
    }
    else if (charType == AnnCharType::CHI_CHAR) {
        model = std::string(svmChiModelPath);
        charSet = chiChars;
    }

    cv::Ptr<cv::ml::SVM> svm_;
    svm_ = cv::ml::SVM::load<cv::ml::SVM>(model);

    auto featMat = CharFeature::getFeatures(inMat,FeatureMethod::HOG);

    float response = svm_->predict(featMat);
    int index = static_cast<int>(response);

    return std::string(charSet[index]);
}

cv::Ptr<ml::TrainData> CxSvm::getTrainData(const int &charCount, const char **charSet)
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
        ss << "  >> Get train characters " << Encode::UTF8ToGBK(m_char) << " in " << subFolder;
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

    cv::Mat samples_;
    samples.convertTo(samples_, CV_32FC1);
    cv::Mat response = cv::Mat::zeros((int)labels.size(), 1, CV_32S);

    for (size_t i = 0; i < labels.size(); i++) {
        response.at<uchar>(i,0) = labels[i];
    }


    return cv::ml::TrainData::create(samples_, cv::ml::SampleTypes::ROW_SAMPLE,response);
}







