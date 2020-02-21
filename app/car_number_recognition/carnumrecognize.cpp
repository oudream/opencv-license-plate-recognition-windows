#include "carnumrecognize.h"

CarNumRecognize::CarNumRecognize()
{

}

CarNumRecognize::~CarNumRecognize()
{

}

int CarNumRecognize::plateLocate(const cv::Mat& src, std::vector<Mat>& resultVec, bool pdMode)
{
    PlateLocate m_plateLocate;

    m_plateLocate.setPdMode(pdMode);

    return m_plateLocate.start(src,resultVec);
}

int CarNumRecognize::charsCutting(const cv::Mat &src, std::vector<Mat> &charsVec, const int &mode)
{
    CharsCutting m_charsCutting;

    m_charsCutting.setCuttingMode(mode);

    return m_charsCutting.start(src,charsVec);
}

int CarNumRecognize::charsRecognize(const std::vector<cv::Mat>& charsVec, std::vector<std::string>& resultVec, const int& mode, const char* lang)
{
    CharsRecognize m_charsRecognize;

    m_charsRecognize.setRecognizeMode(mode);

    if (mode == RecognizeMode::TESS) {
        m_charsRecognize.setTessLang(lang);

        std::vector<cv::Mat> invertCharsVec;

        for (size_t i = 0;i < charsVec.size();i++) {
            cv::Mat tmpMat = charsVec.at(i);

//            cv::threshold(charsVec.at(i),tmpMat, 5, 255, CV_THRESH_BINARY_INV);

//            cv::threshold(charsVec.at(i),tmpMat, 150, 255, CV_THRESH_OTSU|CV_THRESH_BINARY);

//            char tmpTitle[3];
//            sprintf(tmpTitle,"%d",i);
//            cv::imshow(tmpTitle,tmpMat);
            invertCharsVec.push_back(tmpMat);
        }

        return m_charsRecognize.start(invertCharsVec,resultVec);
    }

    return m_charsRecognize.start(charsVec,resultVec);
}

int CarNumRecognize::plateRecognize(const std::string &imageName, std::string &sResult, const int &encode, const int &cuttingMode, const int &recognizeMode)
{
    cv::Mat srcPic = cv::imread(imageName);
    cv::Mat destPic = srcPic.clone();

    IplImage srcImg = srcPic;
    IplImage destImg = destPic;

    ImageAdjust::ContrastAdjust(&srcImg,&destImg,1.1f);

    std::vector<std::vector<std::string> > vecAllResult;

    std::vector<cv::Mat> plateMat;
    int resultPL = CarNumRecognize::plateLocate(destPic,plateMat,true);

    if (resultPL == 0) {

        size_t plateCount = plateMat.size();

        for (size_t i = 0;i < plateCount;i++) {

            IplImage srcImg = plateMat[i];

            cv::Mat destPic = plateMat[i];
            IplImage destImg = destPic;

            ImageAdjust::ContrastAdjust(&srcImg,&destImg,1.2f);

//            char tmpTitle[10];
//            sprintf(tmpTitle,"plate%d",i);
//            cv::imshow(tmpTitle,plateMat[i]);

            std::vector<cv::Mat> charsMat;
            int resultCC = CarNumRecognize::charsCutting(plateMat.at(i),charsMat,cuttingMode);

//            for (int j = 0;j < charsMat.size();j++) {
//                cv::Mat tmpMat = charsMat.at(j);
//                cv::threshold(charsMat.at(j),tmpMat, 5, 255, CV_THRESH_BINARY_INV);

//                char title[10];
//                sprintf(title,"%d_%d",i,j);
//                cv::imshow(title,tmpMat);
//            }

            if (resultCC == 0) {

                std::vector<std::string> vecResult;

                if (recognizeMode == RecognizeMode::BOTH) {
                    for (int n = 0; n < 2; n++) {

                        int m_mode = -1;
                        switch (n) {
                        case 0:
                        {
                            m_mode = RecognizeMode::ANN;
                        }
                            break;
                        case 1:
                        {
                            m_mode = RecognizeMode::TESS;
                        }
                        default:
                            break;
                        }

                        CarNumRecognize::charsRecognize(charsMat,vecResult,m_mode,"plate_chi");
                        vecAllResult.push_back(vecResult);
                    }
                }
                else{
                    CarNumRecognize::charsRecognize(charsMat,vecResult,recognizeMode,"plate_chi");
                    vecAllResult.push_back(vecResult);
                }
            }

            if (vecAllResult.size() < 1) {
                return -1;
            }
            else{
                int ret = CarNumRecognize::getCorrectResult(vecAllResult,sResult);
                if (encode == Encode::GBK) {
                    sResult = Encode::UTF8ToGBK(sResult.c_str());
                }

                return ret;
            }
        }
    }

    //    cv::waitKey(0);
}

int CarNumRecognize::getSingleCharPic(const std::string &image,const std::string &imageName)
{
    std::string imagePath = "tmp/" + imageName;
    mkdir("tmp");
    mkdir(imagePath.c_str());

    cv::Mat srcPic = cv::imread(image);
    cv::Mat destPic = srcPic.clone();

    IplImage srcImg = srcPic;
    IplImage destImg = destPic;

    ImageAdjust::ContrastAdjust(&srcImg,&destImg,1.1f);

    std::vector<cv::Mat> plateMat;
    int resultPL = CarNumRecognize::plateLocate(destPic,plateMat,true);

    if (resultPL == 0) {

        size_t plateCount = plateMat.size();

        for (size_t i = 0;i < plateCount;i++) {

            char tempPlate[10];
            sprintf(tempPlate, "%d", i);

            std::string platePath = "tmp/" + imageName + "/" + std::string(tempPlate);
            mkdir(platePath.c_str());

            IplImage srcImg = plateMat[i];

            cv::Mat destPic = plateMat[i];
            IplImage destImg = destPic;

            ImageAdjust::ContrastAdjust(&srcImg,&destImg,1.2f);

            std::vector<cv::Mat> charsMat;
            int resultCC = CarNumRecognize::charsCutting(plateMat.at(i),charsMat,CuttingMode::CONTOUR_BLOCK);

            std::string charPath;
            size_t charCount = charsMat.size();
            for (size_t j = 0; j < charCount; j++) {
                char tempChar[10];
                sprintf(tempChar, "%d", j);

                charPath = platePath + "/" + std::string(tempChar) + ".jpg";

                cv::imwrite(charPath,charsMat.at(j));
            }
        }
    }
}

int CarNumRecognize::getCorrectResult(const std::vector<std::vector<std::string> > &allResultVec, std::string &sResult)
{
    size_t resultCount = allResultVec.size();

    /* 最后统计出来正确的字符组 */
    std::vector<std::string> correctChars;
    correctChars.reserve(8);
    for (size_t n = 0; n < 7; n++) {
        correctChars.push_back(std::string("#"));
    }

    /* 临时候选字符组 */
    std::vector<std::string> candidatedChars;

    /* 候选字符组对应的个数组 */
    std::vector<size_t> vecCharCount;

    /* 车牌7个字符，每一个字符都进行统计分析 */
    for (size_t n = 0; n < 7; n++) {

        /* 针对识别出来的不同的结果进行单字统计 */
        for (size_t i = 0; i < resultCount; i++) {

            std::vector<std::string> oneOfResult = allResultVec.at(i);

            if (oneOfResult.at(n) == "*") {
                continue;
            }

            size_t candidatedCharCount = candidatedChars.size();
            if (candidatedCharCount == 0) {
                candidatedChars.push_back(oneOfResult.at(n));
                vecCharCount.push_back(1);
            }
            else{
                bool charHasFound = false;

                for (size_t m = 0; m < candidatedCharCount; m++) {
                    if (candidatedChars.at(m) == oneOfResult.at(n)) {
                        vecCharCount[m]++;
                        charHasFound = true;
                    }
                }

                if (charHasFound == false) {
                    candidatedChars.push_back(oneOfResult.at(n));
                    vecCharCount.push_back(1);
                }
            }

        }

        /* 找出出现次数最多的字符（等同于正确率高的字符）*/
        size_t maxNum = 0;
        int index = -1;

        for (size_t m = 0; m < vecCharCount.size(); m++) {
            if (vecCharCount[m] > maxNum) {
                maxNum = vecCharCount[m];
                index = m;
            }
        }

        correctChars[n] = candidatedChars[index];

        candidatedChars.clear();
        vecCharCount.clear();

    }

    if (correctChars.size() != 7) {
        return -1;
    }

    std::vector<std::string>::const_iterator iter = correctChars.begin();
    while (iter != correctChars.end()) {
        sResult.append(*iter);
        ++iter;
    }


    return 0;
}



