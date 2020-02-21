#ifndef PNRTHREAD_H
#define PNRTHREAD_H

#include <QThread>
#include <QMap>

#include "carnumrecognize.h"

class PnrThread : public QThread
{
    Q_OBJECT

public:
    explicit PnrThread();
    ~PnrThread();

protected:
    void run();

private:
    QMap<int,QString> imageList;

signals:
    void sendFinish(int,QString);

public slots:
    void receiveImages(QMap<int,QString> images);

};

#endif // PNRTHREAD_H
