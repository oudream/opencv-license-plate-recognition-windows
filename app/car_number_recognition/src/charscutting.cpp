#include "charscutting.h"


CharsCutting::CharsCutting()
{

}

CharsCutting::~CharsCutting()
{

}

int CharsCutting::start(const cv::Mat &src, std::vector<cv::Mat> &charsVec)
{
    if (m_cuttingMode == CuttingMode::CONTOUR_BLOCK) {
        return cutByContourBlock(src,charsVec);
    }
    else if (m_cuttingMode == CuttingMode::PROJECTION) {
        return cutByProjection(src,charsVec);
    }
}

int CharsCutting::cutByProjection(const cv::Mat &src, std::vector<cv::Mat> &charsVec)
{
    cv::Mat srcGrayPic;
    cv::Mat binPic;

    cv::cvtColor(src,srcGrayPic,CV_BGR2GRAY);

    cv::Mat grayEnhancePic = srcGrayPic.clone();

    IplImage srcGrayImg = srcGrayPic;
    IplImage grayEnhanceImg = grayEnhancePic;

    /* 图像对比度调节 */
    ImageAdjust::ContrastAdjust(&srcGrayImg,&grayEnhanceImg,2.0f);

    cv::threshold(grayEnhancePic,binPic, 50, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);

    if (clearLiuDing(binPic) == false) {
        return -1;
    }

    pixCompensation(binPic,0,1);
//    cv::threshold(binPic,binPic, 100, 255, CV_THRESH_BINARY_INV);

    std::vector<cv::Mat> charsMat;
    int result = splitChar(binPic, charsMat);

    if (result != 0 || charsMat.size() != 7){
        return -1;
    }

    charsVec = charsMat;

    return 0;
}

int CharsCutting::cutByContourBlock(const cv::Mat &src, std::vector<cv::Mat> &charsVec)
{
    std::vector<cv::Mat> resultVec;
    CCharsSegment m_charSegment;

    int result = m_charSegment.charsSegment(src, resultVec);
    if (result == 0) {
        size_t num = resultVec.size();
        if (num < 7) {
            return -1;
        }
        for (size_t n = 0; n < num; n++) {
            cv::Mat resultMat = resultVec[n];
            charsVec.push_back(resultMat);
        }
    }

    return result;
}

int CharsCutting::splitChar(const cv::Mat &src, std::vector<cv::Mat>& out)
{
    int w = src.cols;
    int h = src.rows;

    cv::Mat img_contours;
    src.copyTo(img_contours);

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(img_contours,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

    std::vector<std::vector<cv::Point> >::const_iterator iter = contours.begin();
    std::vector<cv::Rect> vecRect;

    // 将不符合特定尺寸的字符块排除出去

    while (iter != contours.end()) {
        cv::Rect rect = cv::boundingRect(cv::Mat(*iter));
        cv::Mat auxRoi(src, rect);

        if (CheckCharSizes(auxRoi) == true) {
            vecRect.push_back(rect);
        }
        ++iter;
    }

    if (vecRect.size() == 0) return -1;

    std::map<std::string,int> charBorder;

    if (getCharBorder(vecRect,w,charBorder) == -1) {
        return -1;
    }


    int x = 0;
    int y = charBorder["top"];
    int width = w;
    int height = charBorder["bot"] - charBorder["top"];

    cv::Mat tmpMat = src(Rect_<int>(x, y, width, height));

    cv::imwrite("result.jpg",tmpMat);

    std::vector<cv::Rect> charsRect;

    int result = getAllSplitArea(tmpMat,charBorder["width"],charsRect);
    if (result != 0 || charsRect.size() != 7) {
        return -1;
    }
    else{
        for (size_t i = 0; i < charsRect.size(); i++) {

            cv::Rect rect = charsRect.at(i);
            cv::Mat charMat = cv::Mat(tmpMat,rect).clone();

            resizeChar(charMat,charMat);

            out.push_back(charMat);
        }
    }

    return 0;
}

int CharsCutting::getAllSplitArea(const cv::Mat& src, const int& charWidth, std::vector<cv::Rect>& allSplitRect)
{
    size_t w = src.cols;
    size_t h = src.rows;

    size_t pixsOfCols[w];

    for (size_t j = 0; j < w; j++) {
        size_t oneColPixs = 0;
        for (size_t i = 0; i < h; i++) {
            if (src.at<uchar>(i,j) == 255) {
                oneColPixs++;
            }
        }

        pixsOfCols[j] = oneColPixs;
    }

    std::vector<std::map<std::string,size_t> > chars;
    bool start = false;
    size_t x1 = 0;
    size_t x2 = 0;

    for (size_t n = 0; n < w; n++) {
        if (start == false && pixsOfCols[n] > 0) {
            start = true;

            x1 = n;
        }
        else if (start == true && (pixsOfCols[n] == 0 || n == w - 1)) {
            start = false;

            x2 = n;

            std::map<std::string,size_t> charX;
            charX["left"] = x1;
            charX["right"] = x2;

            chars.push_back(charX);
        }
    }



    std::vector<cv::Rect> charsRect;

    cv::Rect firstRect(0,0,charWidth,h);

    charsRect.push_back(firstRect);

    std::vector<size_t> thinCharsIndex;
    for (size_t n = 0; n < chars.size(); n++) {
        std::map<std::string,size_t> charX = chars.at(n);
        if ((charX["right"] - charX["left"]) < 0.8 * charWidth
                && (charX["right"] + charX["left"]) / 2 < w / 7 ) {

            thinCharsIndex.push_back(n);
        }
        else if ((charX["right"] - charX["left"]) < 0.5 * charWidth) {

            cv::Rect rect(charX["left"],0,(charX["right"] - charX["left"]),h);
            cv::Mat charMat = cv::Mat(src,rect).clone();

            int nonZeroArea = cv::countNonZero(charMat);
            int matArea = charMat.cols * charMat.rows;

            float pixPercent = nonZeroArea * 1.0f / matArea;

            if (pixPercent > 0.5) {
                charsRect.push_back(rect);
            }
        }
        else if ((charX["right"] - charX["left"]) > 1.1f * charWidth) {
            cv::Rect rect1(charX["left"],0,(charX["right"] - charX["left"]) / 2,h);
            charsRect.push_back(rect1);

            cv::Rect rect2((charX["right"] + charX["left"]) / 2,0,(charX["right"] - charX["left"]) / 2,h);
            charsRect.push_back(rect2);
        }
        else{
            cv::Rect rect(charX["left"],0,(charX["right"] - charX["left"]),h);
            charsRect.push_back(rect);
        }
    }

    if (thinCharsIndex.size() > 1) {
        bool isFound = false;

        std::map<std::string,size_t> charXLast = chars.at(thinCharsIndex.size() - 1);

        int indexPrev = thinCharsIndex.size() - 2;
        while (indexPrev >= 0) {
            std::map<std::string,size_t> charXPrev = chars.at(indexPrev);

            if ((charXLast["right"] - charXPrev["left"]) >= int(charWidth * 0.8)
                    && (charXLast["right"] - charXPrev["left"]) <= int(charWidth * 1.1)) {

                cv::Rect rect(charXPrev["left"],0,(charXLast["right"] - charXPrev["left"]),h);
                charsRect[0] = rect;
                isFound = true;

                break;
            }

            indexPrev--;
        }

        if (isFound == false) {
            charsRect.erase(charsRect.begin());
        }
    }
    else{
        charsRect.erase(charsRect.begin());
    }

    if (charsRect.size() >= 7) {
        for (size_t n = 0; n < 7; n++) {
            allSplitRect.push_back(charsRect.at(n));
        }
    }
    else{
        return -1;
    }

    return 0;

}

bool CharsCutting::pixCompensation(cv::Mat &src, const int &type, const int &count)
{
    if( count < 1 ){
        return false;
    }

    int w = src.cols;
    int h = src.rows;

    for( int nCount = 0; nCount < count; nCount++ ){
        for( int i = 0; i < h; i++ ){
            for( int j = 0; j < w; j++ ){

                // 上边像素坐标
                int iUp = i - 1;
                int jUp = j;

                // 下边像素坐标
                int iDown = i + 1;
                int jDown = j;

                // 左边像素坐标
                int iLeft = i;
                int jLeft = j - 1;

                // 右边像素坐标
                int iRight = i;
                int jRight = j + 1;

                // 左上边像素坐标
                int iUpL = i - 1;
                int jUpL = j - 1;

                // 左下边像素坐标
                int iDownL = i + 1;
                int jDownL = j - 1;

                // 右上边像素坐标
                int iUpR = i - 1;
                int jUpR = j + 1;

                // 右下边像素坐标
                int iDownR = i + 1;
                int jDownR = j + 1;

                if( iUp < 0 || iDown >= h || jLeft < 0 || jRight >= w ){
                    continue;
                }


                if( src.at<uchar>(i,j) == 255 ){
                    if( src.at<uchar>(iUp,jUp) == 0
                            && src.at<uchar>(iDown,jDown) == 0
                            && src.at<uchar>(iLeft,jLeft) == 0
                            && src.at<uchar>(iRight,jRight) == 0 ){

                        src.at<uchar>(i,j) = 0;
                    }
                    else if( src.at<uchar>(iUp,jUp) == 0
                             && src.at<uchar>(iDown,jDown) == 0
                             && src.at<uchar>(iLeft,jLeft) == 0
                             && src.at<uchar>(iRight,jRight) == 0 ){

                        src.at<uchar>(i,j) = 0;
                    }
                    else if( src.at<uchar>(iUp,jUp) == 0
                             && src.at<uchar>(iDown,jDown) == 0
                             && src.at<uchar>(iLeft,jLeft) == 0
                             && src.at<uchar>(iRight,jRight) == 0 ){

                        src.at<uchar>(i,j) = 0;
                    }
                    else if( src.at<uchar>(iUp,jUp) == 0
                             && src.at<uchar>(iDown,jDown) == 0
                             && src.at<uchar>(iLeft,jLeft) == 0
                             && src.at<uchar>(iRight,jRight) == 0 ){

                        src.at<uchar>(i,j) = 0;
                    }
                }
            }
        }
    }

    return true;
}

bool CharsCutting::CheckCharSizes(cv::Mat r) {
    // Char sizes 45x90
    float aspect = 45.0f / 90.0f;
    float charAspect = (float)r.cols / (float)r.rows;
    float error = 0.7f;
    float minHeight = 10.f;
    float maxHeight = 35.f;
    // We have a different aspect ratio for number 1, and it can be ~0.2
    float minAspect = 0.05f;
    float maxAspect = aspect + aspect * error;
    // area of pixels
    int area = cv::countNonZero(r);
    // bb area
    int bbArea = r.cols * r.rows;
    //% of pixel in area
    int percPixels = area / bbArea;

    if (percPixels <= 1
            && charAspect > minAspect && charAspect < maxAspect
            && r.rows >= minHeight && r.rows < maxHeight) {
        return true;
    }
    else{
        return false;
    }
}

int CharsCutting::getCharBorder(const std::vector<cv::Rect>& vecRect, const int& matWidth, std::map<std::string,int>& charBorder)
{
    /* 先找出车牌中间三个不受铆钉影响的字符 */
    int maxHeight = 0;
    std::vector<size_t> charsIndex;

    for (size_t i = 0; i < vecRect.size(); i++) {
        if (vecRect[i].height > maxHeight) {
            maxHeight = vecRect[i].height;
        }
    }

    for (size_t i = 0; i < vecRect.size(); i++) {
        cv::Rect rect = vecRect[i];
        int midx = rect.x + rect.width / 2;

        //如果一个字符有一定的高度，并且在整个车牌的1/3到7/10之间，则是我们要找的中间三个字符其中之一
        if (rect.height > maxHeight * 0.8 && (midx > int(matWidth / 3) && midx < int(matWidth / 10) * 7)) {
            charsIndex.push_back(i);
        }
    }

    if (charsIndex.size() == 0) return -1;

    /* 根据中间的字符上下边界来确定车牌所有字符的上下边界 */
    int midY = vecRect.at(charsIndex[0]).y + vecRect.at(charsIndex[0]).height / 2;
    int topBorder = midY;
    int botBorder = midY;
    int maxWidth = 0;

    for (size_t i = 0; i < charsIndex.size(); i++) {
        size_t index = charsIndex.at(i);

        int topY = vecRect.at(index).y;
        int botY = vecRect.at(index).y + vecRect.at(index).height;

        if (topY < topBorder) {
            topBorder = topY;
        }

        if (botY > botBorder) {
            botBorder = botY;
        }

        if (vecRect.at(index).width > maxWidth) {
            maxWidth = vecRect.at(index).width;
        }
    }

    charBorder["top"] = topBorder;
    charBorder["bot"] = botBorder;
    charBorder["width"] = maxWidth;

    return 0;
}

void CharsCutting::resizeChar(const cv::Mat& in, cv::Mat& out)
{
  int h = in.rows;
  int w = in.cols;

  //统一每个字符的大小

  int charSize = 20;

  cv::Mat transformMat = cv::Mat::eye(2, 3, CV_32F);
  int m = max(w, h);
  transformMat.at<float>(0, 2) = float(m / 2 - w / 2);
  transformMat.at<float>(1, 2) = float(m / 2 - h / 2);

  cv::Mat warpImage(m, m, in.type());
  cv::warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR,
             BORDER_CONSTANT, cv::Scalar(0));

  //！ 将所有的字符调整成统一的尺寸
  cv::Mat tmpOut;
  cv::resize(warpImage, tmpOut, cv::Size(charSize, charSize));

  cv::Rect rect = GetCenterRect(tmpOut);
  out = cv::Mat(tmpOut,rect);
//  out = CutTheRect(tmpOut,rect);

}

cv::Rect CharsCutting::GetCenterRect(const cv::Mat &in)
{
  Rect _rect;

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


cv::Mat CharsCutting::CutTheRect(const cv::Mat &in, cv::Rect &rect) {

    int h = 20;
    int w = 10;

    cv::Mat tmpIn = cv::Mat(in,rect);

    cv::Mat dstMat(h, w, tmpIn.type());
    dstMat.setTo(Scalar(0));

    if (tmpIn.cols >= dstMat.cols) {
        resize(tmpIn,dstMat,Size(h, w));
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

    //

    return dstMat;
}
