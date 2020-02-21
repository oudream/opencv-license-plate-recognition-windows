#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <string>
#include <direct.h>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

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



#endif // COMMON_H
