#include "pnrthread.h"

PnrThread::PnrThread()
{
    imageList.clear();
}

PnrThread::~PnrThread()
{

}

void PnrThread::run()
{
    while (true) {
        if (imageList.isEmpty() == false) {
            QMap<int, QString>::const_iterator iter = imageList.constBegin();
            while (iter != imageList.constEnd()) {

                std::string sResult;
                int index = iter.key();
                QString qsImage = iter.value();
                std::string imageName = std::string(qsImage.toLocal8Bit().data());

                int iRes = CarNumRecognize::plateRecognize(imageName,sResult,Encode::UTF8,CuttingMode::CONTOUR_BLOCK,RecognizeMode::ANN);
                //            if (iRes == 0) {
                QString qsResult = QString::fromStdString(sResult);

                emit sendFinish(index,qsResult);
                //            }

                ++iter;
            }

            break;
        }

        QThread::msleep(100);
    }
}

void PnrThread::receiveImages(QMap<int, QString> images)
{
    QMap<int, QString>::const_iterator iter = images.constBegin();
    while (iter != images.constEnd()) {
        imageList.insert(iter.key(),iter.value());

        ++iter;
    }
}
