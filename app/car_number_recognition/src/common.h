#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <direct.h>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/core/core.hpp"

#include "tesseract/baseapi.h"
#include "tesseract/strngs.h"
#include "tesseract/renderer.h"

#include "easypr/config.h"
#include "easypr/core/plate_recognize.h"
#include "easypr/core/core_func.h"


using namespace easypr;


namespace CuttingMode {
    const int PROJECTION = 0;
    const int CONTOUR_BLOCK = 1;
}

namespace RecognizeMode {
    const int TESS = 0;
    const int ANN = 1;
    const int BOTH = 2;
}

namespace Encode {
    const int UTF8 = 0;
    const int GBK = 1;

    std::string UTF8ToGBK(const char* str);
    std::string GBKToUTF8(const char* strGBK);
}

namespace AnnCharType {
    const int ENG_CHAR = 0;
    const int CHI_CHAR = 1;
}

namespace FeatureMethod {
    const int PIXELS = 0;
    const int LBP = 1;
    const int HOG = 2;
}


static const char* annEngTrainPath = "train/eng";
static const char* annChiTrainPath = "train/chi";

static const char* annEngModelPath = "model/ann_eng.xml";
static const char* annChiModelPath = "model/ann_chi.xml";
static const char* svmEngModelPath = "model/svm_eng.xml";
static const char* svmChiModelPath = "model/svm_chi.xml";

static const int engCharCount = 34;
static const int chiCharCount = 31;


static const char* engChars[] = {
    /* 10个数字 */
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",

    /* 24个英文字母，除去了"I","O" */
    "A", "B", "C", "D", "E", "F", "G", "H", /*"I",*/
    "J", "K", "L", "M", "N", /*"O",*/ "P", "Q", "R",
    "S", "T", "U", "V", "W", "X", "Y", "Z"
};

static const char* chiChars[] = {
  /*  31个中文代号  */
  "川", "鄂", "赣", "甘", "贵", "桂", "黑", "沪",
  "冀", "津", "京", "吉", "辽", "鲁", "蒙", "闽",
  "宁", "青", "琼", "陕", "苏", "晋", "皖", "湘",
  "新", "豫", "渝", "粤", "云", "藏", "浙"
};

typedef void (*fn_outInfo_t)(const char *);

class Common
{
public:
    static void setFnOutInfo(fn_outInfo_t);

    static void outInfo(const std::string & sInfo);
    static void outInfo(const char * sInfo);

};


#endif // COMMON_H
