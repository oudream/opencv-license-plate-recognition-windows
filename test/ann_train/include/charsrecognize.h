#ifndef CHARSRECOGNIZE_H
#define CHARSRECOGNIZE_H

#include "common.h"

class CharsRecognize
{
public:
    CharsRecognize();
    ~CharsRecognize();

    inline void setRecognizeMode(const int& mode) {m_recognizeMode = mode;}

    inline void setTessLang(const char* lang) {m_tessLang = lang;}

    int start(const std::vector<cv::Mat>& charsVec, std::vector<std::string>& resultVec);

private:
    void recognizeByTess(const std::vector<cv::Mat>& charsVec, std::vector<std::string>& chars);
    void recognizeByAnn(const std::vector<cv::Mat>& charsVec, std::vector<std::string>& chars);

    int checkAndReturnResult(const std::vector<std::string>& chars,std::vector<std::string>& resultVec);

    void cutUselessChar(char* srcStr, char* destStr);

private:
    int m_recognizeMode;
    std::string m_tessLang;
};

#endif // CHARSRECOGNIZE_H
