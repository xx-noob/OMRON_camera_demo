#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OMRON_demo.h"
#include <StApi_TL.h>
#include <StApi_IP.h>
#include <StApi_GUI.h>
#include <Shlobj.h>

using namespace StApi;
class OMRON_demo : public QMainWindow
{
    Q_OBJECT

public:
    OMRON_demo(QWidget *parent = nullptr);
    ~OMRON_demo();

    void ImageShow(const QImage& image);
signals:

    void saveImageRequest(const QString& path);  // 用于触发工作线程保存图像的信号
    void updateWhiteBalance(double value);
    void updateGain(float value);
    void updateExposure(double value);

public slots:
    void onPathButtonClicked();
    void setWhiteBalance(double value);
    void setGain(int value);
    void setExposure(double value);
private:
    Ui::OMRON_demoClass ui;
    QString folderPath;// 用户选择的文件夹路径

};
