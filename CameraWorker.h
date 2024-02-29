#pragma once

#include <QObject>
#include <StApi_TL.h>
#include <StApi_IP.h>
#include <StApi_GUI.h>
#include <Shlobj.h>
using namespace StApi;
using namespace GenApi;
#include <QImage>
#include <QPixmap>
#include<QTimer>
#include"OMRON_demo.h"

#include<opencv.hpp>
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
using namespace cv;

class CameraWorker  : public QObject
{
	Q_OBJECT
		

public:
	CameraWorker(QObject *parent);
	~CameraWorker();
    CameraWorker();


public slots:
	void connectToCamera();
	void disconnectToCamera();
	void captureImage();
	void UpdateCameraImage(IStImage* pIStImage);
	void Enumeration(INodeMap* pINodeMap, const char* szEnumerationName);
	void captureSingleImage();
	void setSaveFolderPath(const QString& folderPath);//保存图像路径以及格式
	void saveImage(const QString& folderPath);
	void onAWBStateChanged(int state);
	void SetWhiteBalance(double value);
	void onAGCStateChanged(int state);
	void SetGain(int value);
	void onAEStateChanged(int state);
	void SetExposure(double value);

signals:
	void displayImage(const  QImage& image);

private:
	CStApiAutoInit objStApiAutoInit;//初始化
	CIStSystemPtr pIStSystem;//系统对象
	CIStDevicePtr pIStDevice;//相机对象
	QTimer* imageCaptureTimer;
	CIStDataStreamPtr pIStDataStream;//图像数据流
	CIStStreamBufferPtr pIStStreamBuffer;//缓冲区指针
	IStImage* pIStImage;//原始图像
	const void* imageDataPtr;
	unsigned char* g_pImageData = nullptr;//存储相机图像数据
	bool stopImageCapture;

	QString saveFolderPath;//文件路径
	

	//用于opencv的图像采集
	Mat inputMat;
	Mat displayMat;
	// 获取相机的 INodeMap 对象
	CNodeMapPtr pINodeMapRemote;
	// 获取曝光时间节点
	CFloatPtr pIExposureTime;
	CEnumerationPtr pIExposureMode;
	CEnumerationPtr pIExposureAuto;
	//获取增益模式节点
	CFloatPtr pIGain;
	CEnumerationPtr pIGainAuto;
	//获取白平衡的模式
	CFloatPtr pIWhiteBalance;
	CEnumerationPtr pIBalanceWhiteAuto;

	
	bool isBalanceWhiteAuto;//白平衡状态
	bool isGainAuto;//增益状态
	bool isExposureTimeAuto=false;//曝光时间状态


	//Feature names
	const char* EXPOSURE_AUTO = "ExposureAuto";			//Standard
	const char* GAIN_AUTO = "GainAuto";					//Standard
	const char* BALANCE_WHITE_AUTO = "BalanceWhiteAuto";	//Standard

	const char* AUTO_LIGHT_TARGET = "AutoLightTarget";		//Custom
	const char* GAIN = "Gain";								//Standard
	const char* GAIN_RAW = "GainRaw";						//Custom

	const char* EXPOSURE_MODE = "ExposureMode";			//Standard
	const char* EXPOSURE_TIME = "ExposureTime";			//Standard
	const char* EXPOSURE_TIME_RAW = "ExposureTimeRaw";		//Custom

	const char* BALANCE_RATIO_SELECTOR = "BalanceRatioSelector";	//Standard
	const char* BALANCE_RATIO = "BalanceRatio";			//Standard

};


