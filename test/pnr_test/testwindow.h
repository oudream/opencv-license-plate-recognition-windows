#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QColor>

#include <QDebug>
#include <QDateTime>

#include "pnrthread.h"
#include "trainthread.h"

namespace Ui {
class testWindow;
}

class testWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit testWindow(QWidget *parent = 0);
    ~testWindow();
    void outInfo(const QString &sInfo);

private:
    void init();
    void buildConnection();

    void calcAccuracyRate();

    void plateCutting(QString image);

    inline void deleteItem(QTableWidgetItem* item){if (item != NULL) {delete item; item = NULL;}}

private:
    Ui::testWindow *ui;

    PnrThread* pnrThread;
    TrainThread* trainThread;

    QList<QTableWidgetItem*> pItemList;

    QMap<int,QString> images;

signals:
    void sendImagesData(QMap<int,QString>);

private slots:
    void startButton_onClick();
    void getTrainButton_onClick();
    void loadAction_onClick(bool checked);
    void quitAction_onClick(bool checked);
    void trainAction_onClick(bool checked);

    void receivePnrResult(int index,QString qsResult);

    void threadFinish();

};

#endif // TESTWINDOW_H
