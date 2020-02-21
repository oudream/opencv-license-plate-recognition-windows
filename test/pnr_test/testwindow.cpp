#include "testwindow.h"
#include "ui_testwindow.h"

testWindow * f_win = 0;

static void f_fnOutInfo(const char * sInfo)
{
    if (f_win)
    {
        f_win->outInfo(QString::fromLocal8Bit(sInfo));
    }
}


testWindow::testWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::testWindow)
{
    ui->setupUi(this);

    f_win = this;
    Common::setFnOutInfo(f_fnOutInfo);

    init();

    trainAction_onClick(true);
    loadAction_onClick(true);
}

testWindow::~testWindow()
{
    Common::setFnOutInfo(0);
    f_win = 0;

    for (int i = 0; i < pItemList.count(); i++) {
        deleteItem(pItemList[i]);
    }

    pItemList.clear();

    if (pnrThread != NULL) {
        delete pnrThread;
        pnrThread = NULL;
    }

    if (trainThread != NULL) {
        delete trainThread;
        trainThread = NULL;
    }

    delete ui;
}

void testWindow::init()
{
    pItemList.clear();
    images.clear();

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->startButton->setEnabled(false);
    buildConnection();
}

void testWindow::buildConnection()
{
    connect(ui->loadAction,SIGNAL(triggered(bool)),this,SLOT(loadAction_onClick(bool)));
    connect(ui->quitAction,SIGNAL(triggered(bool)),this,SLOT(quitAction_onClick(bool)));
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(startButton_onClick()));
    connect(ui->getTrainAction,SIGNAL(triggered(bool)),this,SLOT(getTrainButton_onClick()));
    connect(ui->trainAction,SIGNAL(triggered(bool)),this,SLOT(trainAction_onClick(bool)));
}

void testWindow::calcAccuracyRate()
{
    int correctCount = 0;

    QString compareResult;
    QColor compareResultColor;

    int rowCount = ui->tableWidget->rowCount();


    for (int i = 0; i < rowCount; i++) {
        QString fileName = ui->tableWidget->item(i,0)->text();
        QString pnrResult = ui->tableWidget->item(i,1)->text();

        QString referText = fileName.split(".").at(0);

        if (QString::compare(pnrResult,referText) == 0) {
            correctCount++;

            compareResult = tr("正确");
            compareResultColor = QColor(0,175,80);
        }
        else{
            compareResult = tr("错误");
            compareResultColor = QColor(255,0,0);
        }

        QTableWidgetItem* item = new QTableWidgetItem(compareResult);
        item->setTextColor(compareResultColor);
        ui->tableWidget->setItem(i,2,item);

        pItemList.append(item);
    }

    float rate = float(correctCount * 100.0 / rowCount );

    QString numLabelText = QString::number(correctCount) + tr("/") + QString::number(rowCount);
    ui->numLabel->setText(numLabelText);

    ui->accuracyLabel->setText(QString::number(rate) + tr("%"));

}

void testWindow::plateCutting(QString image)
{
    QFileInfo fileInfo(image);
    QString plateFolderName = fileInfo.fileName().split(".").at(0);

    std::string imagePath = std::string(image.toLocal8Bit().data());
    std::string imageName = std::string(plateFolderName.toLocal8Bit().data());

    CarNumRecognize::getSingleCharPic(imagePath,imageName);
}

/*
 * slot函数
 */
void testWindow::startButton_onClick()
{
    pnrThread = new PnrThread();

    connect(this,SIGNAL(sendImagesData(QMap<int,QString>)),pnrThread,SLOT(receiveImages(QMap<int,QString>)));
    connect(pnrThread,SIGNAL(sendFinish(int,QString)),this,SLOT(receivePnrResult(int,QString)));
    connect(pnrThread,SIGNAL(finished()),this,SLOT(threadFinish()));

    emit sendImagesData(images);

    pnrThread->start();

    ui->startButton->setEnabled(false);
}

void testWindow::getTrainButton_onClick()
{
    QMap<int,QString>::const_iterator iter = images.constBegin();

    while (iter != images.constEnd()) {
        plateCutting(iter.value());

        ++iter;
    }
}

void testWindow::loadAction_onClick(bool checked)
{
    bool bStartButtonEnabled = ui->startButton->isEnabled();
    ui->startButton->setEnabled(false);

    QStringList absFiles;
    if (1>0)
    {
        QString sCarPath = QApplication::applicationDirPath() + "\\car\\";
        QDir dir(sCarPath);
        if(!dir.exists())
        {
            return;
        }
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList list = dir.entryInfoList();
        if(list.count() <= 0)
        {
            return;
        }
        for (size_t i = 0; i < list.size(); ++i)
        {
            QFileInfo file_info = list.at(i);
            QString suffix = file_info.suffix();
            if(QString::compare(suffix, QString("jpg"), Qt::CaseInsensitive) == 0)
            {
                QString absolute_file_path = file_info.absoluteFilePath();
                absFiles.append(absolute_file_path);
            }
        }
    }
    else
    {
        absFiles = QFileDialog::getOpenFileNames(this,tr("选择图片"),QDir::homePath(),"Images (*.jpg *.png)");
    }

    if (absFiles.empty()) return;

    QStringList fileNames;
    for (int i = 0; i < pItemList.count(); i++) {
        deleteItem(pItemList[i]);
    }

    pItemList.clear();

    images.clear();

    QStringList::const_iterator iter = absFiles.constBegin();
    while (iter != absFiles.constEnd()) {
        QFileInfo fileInfo(*iter);
        fileNames.append(fileInfo.fileName());

        ++iter;
    }

    int fileCount = fileNames.count();

    if (fileCount == 0) {
        return;
    }

    ui->tableWidget->setRowCount(fileCount);

//    QTableWidget * oGrid = ui->tableWidget;
//    QStringList sHeaders;
//    sHeaders.append(QString::fromUtf8("待识别图片"));
//    sHeaders.append(QString::fromUtf8("识别结果"));
//    sHeaders.append(QString::fromUtf8("是否正确"));
//    int iColumnCount = sHeaders.count();
//    oGrid->setColumnCount(iColumnCount);
//    oGrid->setHorizontalHeaderLabels(sHeaders);
//    oGrid->setColumnWidth(0, 180);
//    oGrid->setColumnWidth(1, 60);
//    oGrid->setColumnWidth(2, 80);

    for (int i = 0; i < fileCount;i++) {
        QTableWidgetItem* item = new QTableWidgetItem(fileNames.at(i));
        ui->tableWidget->setItem(i,0,item);

        pItemList.append(item);

        images.insert(i,absFiles.at(i));
    }

    ui->startButton->setEnabled(bStartButtonEnabled);

    outInfo(QString("加载要识别的图片%1个！").arg(absFiles.size()));
}

void testWindow::quitAction_onClick(bool checked)
{
    this->close();
}

void testWindow::trainAction_onClick(bool checked)
{
    ui->startButton->setEnabled(false);

    outInfo(QString("模型训练开始……请等待训练完成……"));

    trainThread = new TrainThread;

    connect(trainThread,SIGNAL(finished()),this,SLOT(threadFinish()));

    trainThread->start();

    ui->trainAction->setEnabled(false);
}

void testWindow::receivePnrResult(int index, QString qsResult)
{
    QTableWidgetItem* item = new QTableWidgetItem(qsResult);
    ui->tableWidget->setItem(index,1,item);

    pItemList.append(item);
}

void testWindow::threadFinish()
{
    QString resultText;
    if (sender() == pnrThread) {
        calcAccuracyRate();

        delete pnrThread;
        pnrThread = NULL;

        resultText = tr("识别完成！");
        ui->startButton->setEnabled(true);
    }
    else if (sender() == trainThread) {
        delete trainThread;
        trainThread = NULL;

        resultText = tr("训练完成！");
        ui->trainAction->setEnabled(true);
    }

    QMessageBox::information(this,tr("完成"),resultText);

    outInfo(QString("模型训练完成。"));
    ui->startButton->setEnabled(true);
}

void testWindow::outInfo(const QString &sInfo)
{
    ui->plainTextEdit->appendPlainText((QDateTime::currentDateTime().toString() + sInfo));
}

