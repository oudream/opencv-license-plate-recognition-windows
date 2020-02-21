#include "carnumrecognize.h"



int main(int argc, char *argv[])
{
    std::string sResult;
    int result = CarNumRecognize::plateRecognize("test.jpg",sResult,CuttingMode::CONTOUR_BLOCK);

    if (result == 0) {
        std::cout << sResult << std::endl;
    }

    cv::waitKey(0);
    return 0;
}
