#include "charsrecognize.h"

CharsRecognize::CharsRecognize()
{

}

CharsRecognize::~CharsRecognize()
{

}

int CharsRecognize::start(const std::vector<cv::Mat> &charsVec, std::vector<std::string>& resultVec)
{
    std::vector<std::string> tmpChars;

    if (m_recognizeMode == RecognizeMode::TESS) {
//        std::cout << "Tesseract Recognize Mode!" << std::endl;
        recognizeByTess(charsVec,tmpChars);
    }
    else if (m_recognizeMode == RecognizeMode::ANN) {
//        std::cout << "ANN Recognize Mode!" << std::endl;
        recognizeByAnn(charsVec,tmpChars);
    }

    return checkAndReturnResult(tmpChars,resultVec);

}

void CharsRecognize::recognizeByTess(const std::vector<cv::Mat> &charsVec, std::vector<std::string>& chars)
{
    chars.clear();

    tesseract::TessBaseAPI api;

    api.Init(NULL, m_tessLang.c_str(), tesseract::OEM_TESSERACT_ONLY);

//    api.SetVariable("classify_bln_numeric_mode","1");

    api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);

    api.SetImage((uchar*)charsVec[0].data,charsVec[0].cols,charsVec[0].rows,1,charsVec[0].cols);
    char* outUTF8 = api.GetUTF8Text();

    char out[strlen(outUTF8)];

    cutUselessChar(outUTF8,out);

    std::string tmpChar = std::string(out);
    if (tmpChar.empty() == true) {
        chars.push_back(std::string("*"));
    }
    else{
        chars.push_back(tmpChar);
    }

    /* 用英文数字库识别对应的英文数字 */
    api.Init(NULL, "plate_eng", tesseract::OEM_TESSERACT_ONLY);

    api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);

    for (size_t i = 1;i < charsVec.size();i++) {
        api.SetImage((uchar*)charsVec[i].data,charsVec[i].cols,charsVec[i].rows,1,charsVec[i].cols);
        outUTF8 = api.GetUTF8Text();

        char numOut[strlen(outUTF8)];

        cutUselessChar(outUTF8,numOut);

        tmpChar = std::string(numOut);
        if (tmpChar.empty() == true) {
            chars.push_back(std::string("*"));
        }
        else{
            chars.push_back(tmpChar);
        }
    }
}

void CharsRecognize::recognizeByAnn(const std::vector<cv::Mat> &charsVec, std::vector<std::string>& chars)
{
    chars.clear();

    std::string tmpChar;

    size_t vecCount = charsVec.size();

    for (size_t i = 0; i < vecCount; i++) {
        if (i == 0) {
            tmpChar = CxAnn::dataPredict(charsVec[i],AnnCharType::CHI_CHAR);
//            tmpChar = CxSvm::dataPredict(charsVec[i],AnnCharType::CHI_CHAR);
        }
        else{
            tmpChar = CxAnn::dataPredict(charsVec[i],AnnCharType::ENG_CHAR);
//            tmpChar = CxSvm::dataPredict(charsVec[i],AnnCharType::ENG_CHAR);
        }

//        std::string tmpCharUtf8 = Encode::GBKToUTF8(character.second.c_str());
        chars.push_back(tmpChar);
    }
}

int CharsRecognize::checkAndReturnResult(const std::vector<std::string> &chars, std::vector<std::string>& resultVec)
{
    std::string _debugStr;
    bool hasError = false;

    for (size_t i = 0; i < chars.size(); i++) {
        if (chars.at(i) == std::string("*")) {
            hasError = true;
            // TODO
        }

        resultVec.push_back(chars.at(i));
        _debugStr.append(chars.at(i));
    }

    if (hasError == true) {
        return -1;
    }

    std::cout << _debugStr << std::endl;

    return 0;
}

void CharsRecognize::cutUselessChar(char *srcStr, char *destStr)
{
    char* pSrc = srcStr;
    char* pDest = destStr;

    for( int i = 0; i < strlen(srcStr); i++ ){
        if( *pSrc != '\n' ){
            *pDest = *pSrc;
            pDest++;
        }
        pSrc++;
    }

    *pDest = '\0';
}

