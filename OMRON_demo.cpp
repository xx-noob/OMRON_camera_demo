#include "OMRON_demo.h"
#include<QDebug>
#include"CameraWorker.h"
#include<QThread>
#include<QTimer>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>

OMRON_demo::OMRON_demo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	
	//创建子线程对象
	QThread* T1 = new QThread;

	//创建任务类对象	 
	CameraWorker* camerawork = new CameraWorker;

	camerawork->moveToThread(T1);

	T1->start();



	connect(ui.ConnectBtn, &QPushButton::clicked, camerawork, &CameraWorker::connectToCamera);
	connect(ui.DisconnectBtn, &QPushButton::clicked, camerawork, &CameraWorker::disconnectToCamera);
	connect(camerawork, &CameraWorker::displayImage,this,&OMRON_demo::ImageShow, Qt::QueuedConnection);
	//白平衡
	connect(ui.whiteBalanceDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &OMRON_demo::setWhiteBalance);
	connect(ui.AWB_btn, &QCheckBox::stateChanged, camerawork, &CameraWorker::onAWBStateChanged);
	connect(this, &OMRON_demo::updateWhiteBalance, camerawork, &CameraWorker::SetWhiteBalance);
	//增益
	connect(ui.gainSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &OMRON_demo::setGain);
	connect(ui.AGC_btn, &QCheckBox::stateChanged, camerawork, &CameraWorker::onAGCStateChanged);
	connect(this, &OMRON_demo::updateGain, camerawork, &CameraWorker::SetGain);
	//曝光时间
	connect(ui.exposureDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &OMRON_demo::setExposure);
	connect(ui.AE_btn, &QCheckBox::stateChanged, camerawork, &CameraWorker::onAEStateChanged);
	connect(this, &OMRON_demo::updateExposure, camerawork, &CameraWorker::SetExposure);
	// 连接文件自定义路径选取按钮
	connect(ui.pathbtn, &QPushButton::clicked, this, &OMRON_demo::onPathButtonClicked);
	connect(this, &OMRON_demo::saveImageRequest, camerawork, &CameraWorker::setSaveFolderPath);
	//拍照按钮
	connect(ui.captureSinglebtn, &QPushButton::clicked, camerawork, &CameraWorker::captureSingleImage);
	
}

OMRON_demo::~OMRON_demo()
{

}

//显示图像
void OMRON_demo::ImageShow(const QImage& image)
{
	ui.displayLabel->setPixmap(QPixmap::fromImage(image).scaled(ui.displayLabel->size(), Qt::KeepAspectRatio));
}

//文件路径创建
void OMRON_demo::onPathButtonClicked() 
{
	folderPath = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::homePath());
	if (!folderPath.isEmpty()) {
		qDebug() << "Selected folder path: " << folderPath;
		QMessageBox::information(this, "Folder Selected", "Selected folder path: " + folderPath);

		emit saveImageRequest(folderPath); // 发出信号
	}
	else {
		QMessageBox::critical(this, "Error", "No folder selected.");
	}
}


void OMRON_demo::setWhiteBalance(double value) {
	ui.whiteBalanceDoubleSpinBox->setMaximum(511);
	ui.whiteBalanceDoubleSpinBox->setValue(value);
	emit updateWhiteBalance(value);
}

void OMRON_demo::setGain(int value) {
	ui.gainSpinBox->setMaximum(240);
	ui.gainSpinBox->setValue(value);
	emit updateGain(static_cast<float>(value));
}

void OMRON_demo::setExposure(double value) {
	ui.exposureDoubleSpinBox->setMinimum(5967);
	ui.exposureDoubleSpinBox->setMaximum(1276828);
	ui.exposureDoubleSpinBox->setValue(value);
	emit updateExposure(value);
}

