#ifndef TRAINTHREAD_H
#define TRAINTHREAD_H

#include <QThread>

#include "cxann.h"
#include "cxsvm.h"

class TrainThread : public QThread
{
public:
    TrainThread();
    ~TrainThread();

protected:
    void run();

private:
    CxAnn* engAnn;
    CxAnn* chiAnn;

//    CxSvm* engSvm;
//    CxSvm* chiSvm;

};

#endif // TRAINTHREAD_H
