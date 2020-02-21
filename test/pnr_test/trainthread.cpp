#include "trainthread.h"

TrainThread::TrainThread()
{

}

TrainThread::~TrainThread()
{
    if (chiAnn != NULL) {
        delete chiAnn;
        chiAnn = NULL;
    }

    if (engAnn != NULL) {
        delete engAnn;
        engAnn = NULL;
    }

//    if (chiSvm != NULL) {
//        delete chiSvm;
//        chiSvm = NULL;
//    }

//    if (engSvm != NULL) {
//        delete engSvm;
//        engSvm = NULL;
//    }
}

void TrainThread::run()
{
    chiAnn = new CxAnn(AnnCharType::CHI_CHAR);
    chiAnn->dataTrain();

    engAnn = new CxAnn(AnnCharType::ENG_CHAR);
    engAnn->dataTrain();

//    chiSvm = new CxSvm(AnnCharType::CHI_CHAR);
//    chiSvm->dataTrain();

//    engSvm = new CxSvm(AnnCharType::ENG_CHAR);
//    engSvm->dataTrain();
}
