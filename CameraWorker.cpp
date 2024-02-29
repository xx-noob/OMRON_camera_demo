#include "CameraWorker.h"
#include<QDebug>
#include<iostream>
#include <chrono>
#include <iomanip>
#include <QString>
#include <QDateTime>
#include<QFileDialog>

using namespace std;
using namespace cv;
CameraWorker::CameraWorker()
{
   
    
    imageCaptureTimer = new QTimer(this);
    connect(imageCaptureTimer, &QTimer::timeout, this, &CameraWorker::captureImage);
    
}

CameraWorker::~CameraWorker()
{
    if (imageCaptureTimer)
    {
        imageCaptureTimer->stop();
        delete imageCaptureTimer;
        imageCaptureTimer = nullptr;
    }
    
}

void CameraWorker::connectToCamera()
{
    //初始化
    qDebug() << "connectToCamera function is be called" << endl;
    objStApiAutoInit;
    try
    {
    // 创建用于设备扫描和连接的系统对象。
    pIStSystem = CreateIStSystem();
    if (!pIStSystem)
    {
        qDebug() << "Failed to create StApi system." << endl;
        return;
    }
    else
    {
        qDebug() << "Succed to create StApi system." << endl;
    }
    // 创建相机设备对象并连接到第一个检测到的设备。
    pIStDevice = pIStSystem->CreateFirstIStDevice();
    // 创建 INodeMap 对象以访问相机的当前设置。
    pINodeMapRemote=pIStDevice->GetRemoteIStPort()->GetINodeMap();
     //获取曝光模式节点
    pIExposureMode=pINodeMapRemote->GetNode(EXPOSURE_MODE);
    pIExposureAuto= pINodeMapRemote->GetNode(EXPOSURE_AUTO);
    if (IsWritable(pIExposureMode)) {

        // 获取曝光模式的整数值
        int64_t exposureModeValue = pIExposureMode->GetIntValue();
        qDebug() << "Exposure Mode Value: " << exposureModeValue;
        pIExposureMode->SetIntValue(1);
    }
    //获取白平衡模式的节点
     pIBalanceWhiteAuto=pINodeMapRemote->GetNode(BALANCE_WHITE_AUTO);
     //获取增益模式节点
     pIGainAuto = pINodeMapRemote->GetNode(GAIN_AUTO);

    if (pIStDevice)
    {
        qDebug() << "Device=" << pIStDevice->GetIStDeviceInfo()->GetDisplayName() << endl;
        // 检查是否成功创建数据流对象
        pIStDataStream = pIStDevice->CreateIStDataStream(0); // 使用数据流索引 0
        //captureImage();
        pIStDataStream->StartAcquisition();
        pIStDevice->AcquisitionStart();
        imageCaptureTimer->start(30);

    }

    }
    catch(const GenICam_3_2_Sentech::RuntimeException& e)
    {
        // 捕获 StApi 库的异常
        qDebug() << "StApi Exception: " << e.what() << endl;
    }
    catch (const std::exception& e)
    {
        // 捕获其他异常
        qDebug() << "Exception: " << e.what() << endl;
    }
    catch (...)
    {
        // 捕获未知异常
        qDebug() << "Unknown Exception.1" << endl;
    }
}

void CameraWorker::disconnectToCamera()
{

    qDebug() << "disconnectToCamera function is be called" << endl;
    imageCaptureTimer->stop();

    try
    {
     
        if (pIStDataStream)
        {
            // 停止主机端的图像采集。
            pIStDataStream->StopAcquisition();
        }
        if (pIStDevice)
        {
            // 停止相机端的图像采集。
            pIStDevice->AcquisitionStop();
        }
        // 释放数据流资源
        if (pIStDataStream)
        {
            //pIStDataStream->Release();
            pIStDataStream = nullptr;
        }

        // 释放相机资源
        if (pIStDevice)
        {
            //pIStDevice->Release();
            pIStDevice = nullptr;
        }

        // 释放系统对象资源
        if (pIStSystem)
        {
            //pIStSystem->Release();
            pIStSystem = nullptr;
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what() << endl;
    }

}

//void CameraWorker::captureImage()
//{
//    pIStDataStream->StartAcquisition();
//    pIStDevice->AcquisitionStart();
//    try
//    {
//        while (pIStDataStream->IsGrabbing() )
//        {
//            CIStStreamBufferPtr pIStStreamBuffer(pIStDataStream->RetrieveBuffer(5000));
//            if (!pIStStreamBuffer)
//            {
//                qDebug() << "pIStStreamBuffer is nullptr !" << endl;
//                continue;
//            }
//
//            if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
//            {
//                //获取到当前流缓冲区中的图像
//                pIStImage = pIStStreamBuffer->GetIStImage();
//
//                // 获取图像数据的指针
//                imageDataPtr = pIStImage->GetImageBuffer();
//                UpdateCameraImage(pIStImage);
//
//            }
//          
//        }
//    }
//    catch (const GenICam::GenericException& e)
//    {
//        // 处理异常
//        qDebug() << "An exception occurred in captureImage()." << endl << e.GetDescription() << endl;
//    }
//    catch (const std::exception& e)
//    {
//        // 捕获其他异常
//        qDebug() << "Exception: " << e.what() << endl;
//    }
//    catch (...)
//    {
//        // 捕获未知异常
//        qDebug() << "Unknown Exception.2" << endl;
//    }
//}


void CameraWorker::captureImage()
{
   
    try
    {
       
            pIStStreamBuffer=pIStDataStream->RetrieveBuffer(5000);
            if (!pIStStreamBuffer)
            {
                qDebug() << "pIStStreamBuffer is nullptr !" << endl;
            }

            if (pIStStreamBuffer->GetIStStreamBufferInfo()->IsImagePresent())
            {
                //获取到当前流缓冲区中的图像
                pIStImage = pIStStreamBuffer->GetIStImage();

                // 获取图像数据的指针
                imageDataPtr = pIStImage->GetImageBuffer();
                UpdateCameraImage(pIStImage);

            }

    }
    catch (const GenICam::GenericException& e)
    {
        // 处理异常
        qDebug() << "An exception occurred in captureImage()." << endl << e.GetDescription() << endl;
    }
    catch (const std::exception& e)
    {
        // 捕获其他异常
        qDebug() << "Exception: " << e.what() << endl;
    }
    catch (...)
    {
        // 捕获未知异常
        qDebug() << "Unknown Exception.2" << endl;
    }
}

void CameraWorker::UpdateCameraImage(IStImage* pIStImage)
{
    // 检查画面数据是否有效
    if (!pIStImage)
    {
        qDebug() << "No Image";
        return;
    }
    try
    {
        // 获取图像格式
        const StApi::EStPixelFormatNamingConvention_t ePFNC = pIStImage->GetImagePixelFormat();
        StApi::IStPixelFormatInfo* const pIStPixelFormatInfo = StApi::GetIStPixelFormatInfo(ePFNC);
       /* qDebug() << "Is Mono: " << pIStPixelFormatInfo->IsMono();
        qDebug() << "Is Bayer: " << pIStPixelFormatInfo->IsBayer();
        qDebug() << "Each Component Total Bit Count: " << pIStPixelFormatInfo->GetEachComponentTotalBitCount();*/
        
        if (pIStPixelFormatInfo->IsMono() || pIStPixelFormatInfo->IsBayer())
        {
            // 检查图像的大小。
            const size_t nImageWidth = pIStImage->GetImageWidth();
            const size_t nImageHeight = pIStImage->GetImageHeight();
            int nInputType = CV_8UC1;
            if (8 < pIStPixelFormatInfo->GetEachComponentTotalBitCount())
            {
                nInputType = CV_16UC1; 
            }

            // 为输入图像创建 OpenCV 缓冲区。
            if ((inputMat.cols != nImageWidth) || (inputMat.rows != nImageHeight) || (inputMat.type() != nInputType))
            {
                inputMat.create(nImageHeight, nImageWidth, nInputType);
            }
            // 将输入图像数据复制到 OpenCV 缓冲区。
            const size_t dwBufferSize = inputMat.rows * inputMat.cols * inputMat.elemSize() * inputMat.channels();
            memcpy(inputMat.ptr(0), pIStImage->GetImageBuffer(), dwBufferSize);

            // 如果需要，转换像素格式。
            if (pIStPixelFormatInfo->IsBayer())
            {
                int nConvert = 0;
                switch (pIStPixelFormatInfo->GetPixelColorFilter())
                {
                case(StPixelColorFilter_BayerRG): nConvert = COLOR_BayerRG2RGB;    break;
                case(StPixelColorFilter_BayerGR): nConvert = COLOR_BayerGR2RGB;    break;
                case(StPixelColorFilter_BayerGB): nConvert = COLOR_BayerGB2RGB;    break;
                case(StPixelColorFilter_BayerBG): nConvert = COLOR_BayerBG2RGB;    break;
                }
                if (nConvert != 0)
                {
                    /*qDebug() << "PixelColorFilter: " << pIStPixelFormatInfo->GetPixelColorFilter();
                    qDebug() << "Selected Convert: " << nConvert;*/
                    cv::cvtColor(inputMat, displayMat, nConvert);
                   /* qDebug() << "After Color Conversion: "
                        << QString::number(displayMat.at<cv::Vec3b>(0, 0)[0]) << ", "
                        << QString::number(displayMat.at<cv::Vec3b>(0, 0)[1]) << ", "
                        << QString::number(displayMat.at<cv::Vec3b>(0, 0)[2]);*/
                }
                else
                {
                    displayMat = inputMat.clone();  //不需要转换
                }
            }
            else
            {
                displayMat = inputMat.clone();  // 不需要转换
            }
            // 调整图像大小以适应 QLabel 的尺寸
            cv::resize(displayMat, displayMat, cv::Size(640, 400));
           /* qDebug() << "Pixel values: " << QString::number(displayMat.at<cv::Vec3b>(0, 0)[0]) << ", "
                << QString::number(displayMat.at<cv::Vec3b>(0, 0)[1]) << ", "
                << QString::number(displayMat.at<cv::Vec3b>(0, 0)[2]);*/
            // 创建用于显示图像的 QImage
            QImage image(displayMat.data, displayMat.cols, displayMat.rows, QImage::Format_RGB888);
           /* qDebug() << "After Color Conversion - First Pixel: "
                << QString::number(displayMat.at<cv::Vec3b>(0, 0)[0]) << ", "
                << QString::number(displayMat.at<cv::Vec3b>(0, 0)[1]) << ", "
                << QString::number(displayMat.at<cv::Vec3b>(0, 0)[2]);*/

            if (!image.isNull())
            {
                // 发送信号以显示图像
                emit displayImage(image);
                //QMetaObject::invokeMethod(this, "displayImage", Qt::QueuedConnection, Q_ARG(QImage, image));
            }
            else
            {
                qDebug() << "Invalid image data.";
            }
        }
    }
    catch (const GenICam::GenericException& e)
    {
        // 处理 GenICam 异常
        qDebug() << "An exception occurred: " << e.GetDescription();
    }
    catch (const std::exception& e)
    {
        // 处理其他 C++ 异常
        qDebug() << "An exception occurred:  " << e.what();
    }
    catch (...)
    {
        // 处理未知异常
        qDebug() << "An unknown exception occurred.";
    }   
    // 创建 QImage 对象，用于显示画面
    //QImage image(static_cast<uchar*>(const_cast<void*>(pIStImage->GetImageBuffer())), pIStImage->GetImageWidth()/3, pIStImage->GetImageHeight()/3, QImage::Format_RGB888);
   
}

void CameraWorker::setSaveFolderPath(const QString& folderPath)
{
    saveFolderPath = folderPath;
    qDebug() << "saveFolderPath:" << saveFolderPath << endl;
}


void CameraWorker::captureSingleImage()
{
    qDebug() << "captureSingleImage function is be called." << endl;
    try
    {
        saveImage(saveFolderPath);
    }
    catch (const GenICam::GenericException& e)
    {
        // 显示错误信息
        cerr << "An exception occurred." << endl << e.GetDescription() << endl;
    }
}


void CameraWorker::saveImage(const QString& folderPath)
{
    qDebug() << "saveImage function is be called." << endl;
    qDebug() << "saveFolderPath:" << saveFolderPath << endl;
        try
        {
            //// 检查 pIStImage 是否为空指针
            //if (!pIStImage)
            //{
            //    qDebug() << "pIStImage is nullptr. Cannot save image." << endl;
            //    return;
            //}

            //// 创建一个图像保存处理对象
            //CIStStillImageFilerPtr pIStStillImageFiler = CreateIStFiler(StFilerType_StillImage);

            //// 获取当前时间并格式化为字符串
            //QDateTime currentDateTime = QDateTime::currentDateTime();
            //QString timestamp = currentDateTime.toString("yyyyMMdd_hhmmss");

            //// 构建完整的文件路径
            //QString fileName = timestamp + ".tif";
            //QString filePath = folderPath + "/" + fileName;
            //GenICam::gcstring strFilePath(filePath.toStdWString().c_str());

            //// 保存图像到文件（默认为 "TIF" 格式）
            //pIStStillImageFiler->Save(pIStImage, StStillImageFileFormat_TIFF, strFilePath);

            //// 将StApi图像保存到文件
            //std::wcout << L"Saving " << strFilePath.c_str() << L"... ";
            //std::cout << "done" << std::endl;


            // 检查图像缓冲区是否已经有数据
            qDebug() << "Saving image. Save folder path: " << saveFolderPath;  // 检查地址
            if (displayMat.empty()) {
                qDebug() << "Image buffer is empty.";
                
                return;
            }

            qDebug() << "Buffer retrieved successfully.";

            // 检查文件夹路径是否有效
            if (saveFolderPath.isEmpty() || saveFolderPath == "") {
               
                qDebug() << "Please select a folder to save the images.";
                return;

            }

            // 构造图像文件路径，这里使用当前时间作为文件名
            QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
            QString filePath = QDir(saveFolderPath).filePath("image_" + currentDateTime + ".tif");
            qDebug() << "Saving image to: " << filePath;

            // 获取图像数据和大小
            QImage image(displayMat.data, displayMat.cols, displayMat.rows, QImage::Format_RGB888);

            // 使用 QImage 保存图像到文件
            if (image.save(filePath)) {
                qDebug() << "Image saved successfully. Path: " << filePath;
                //emit imageSaved("Image saved successfully. Path: " + filePath);
            }
            else {
                qDebug() << "Failed to save the image.";
                //emit errorOccurred("Failed to save the image.");
            }
        }
        catch (const GenICam::GenericException& e)
        {
            // 显示错误信息
            cerr << "An exception occurred." << endl << e.GetDescription() << endl;
        }
}


void CameraWorker::Enumeration(INodeMap* pINodeMap, const char* szEnumerationName)
{
    // 获取 IEnumeration 接口指针
    CEnumerationPtr pIEnumeration(pINodeMap->GetNode(szEnumerationName));
    if (IsWritable(pIEnumeration))
    {
        // 获取设置列表。
        GenApi::NodeList_t sNodeList;
        pIEnumeration->GetEntries(sNodeList);
        for (;;)
        {
            // 显示可配置的选项。
            qDebug() << szEnumerationName << endl;
            for (size_t i = 0; i < sNodeList.size(); i++)
            {
                if (IsAvailable(sNodeList[i]))
                {
                    CEnumEntryPtr pIEnumEntry(sNodeList[i]);
                    qDebug() << i << " : " << pIEnumEntry->GetSymbolic();
                    // 为当前设置添加“(Current)”。
                    if (pIEnumeration->GetIntValue() == pIEnumEntry->GetValue())
                    {
                        qDebug() << "(Current)";
                    }
                    qDebug() << endl;
                }
            }
            qDebug() << "Select : ";

            // 等待输入。
            size_t nIndex;
            cin >> nIndex;

            // 反映输入的值。
            if (nIndex < sNodeList.size())
            {
                CEnumEntryPtr pIEnumEntry(sNodeList[nIndex]);
                pIEnumeration->SetIntValue(pIEnumEntry->GetValue());
                break;
            }
        }
    }
}



void CameraWorker::SetWhiteBalance(double value)
{
    try {
        if (!isBalanceWhiteAuto) {  // 仅在手动模式下执行
          // 获取白平衡节点
            pIWhiteBalance=pINodeMapRemote->GetNode(BALANCE_RATIO);

            // 设置白平衡值
            if (IsWritable(pIWhiteBalance)) {
                pIWhiteBalance->SetValue(value);
            }
        }
    }
    catch (const GenICam::GenericException& e) {
        qDebug() << "An exception occurred while setting white balance: " << e.GetDescription() << endl;
    }
}

void CameraWorker::SetGain(int value)
{
    qDebug() << "SetGain function is be called" << endl;
    try {
        if (!isGainAuto) {  // 仅在手动模式下执行
    // 获取增益节点
             pIGain=pINodeMapRemote->GetNode(GAIN);

            // 设置增益值
            if (IsWritable(pIGain)) {
                pIGain->SetValue(value);
            }
        }
}
catch (const GenICam::GenericException& e) {
    qDebug() << "An exception occurred while setting gain: " << e.GetDescription() << endl;
}
}

void CameraWorker::SetExposure(double value)
{
    qDebug() << "SetExposure function is be called" << endl;
    try {
        if (!isExposureTimeAuto) {  // 仅在手动模式下执行
        // 获取曝光时间节点
         pIExposureTime=pINodeMapRemote->GetNode(EXPOSURE_TIME);
       
        // 检查是否存在曝光时间节点
        if (pIExposureTime != NULL) {
            // 设置曝光时间值
            if (IsWritable(pIExposureTime)) {
                pIExposureTime->SetValue(value);
            }
           }
        else {
            // 如果曝光时间节点不存在，可以根据需要选择使用 EXPOSURE_TIME_RAW 节点
            qDebug() << "ExposureTime node does not exist. You may need to use EXPOSURE_TIME_RAW if available." << endl;
        }
        }
    }
    catch (const GenICam::GenericException& e) {
        qDebug() << "An exception occurred while setting exposure: " << e.GetDescription() << endl;
    }
}

//白平衡的模式选择
void CameraWorker::onAWBStateChanged(int state)
{
    qDebug() << "onAWBStateChanged function is be called" << endl;
    // 列举白平衡自动模式
     //Enumeration(pINodeMapRemote, BALANCE_WHITE_AUTO);
    if (IsWritable(pIBalanceWhiteAuto)) {
        int64_t newBalanceValue = 0;  // 手动模式

        if (state == Qt::Checked) {
            isExposureTimeAuto = true;
            newBalanceValue = 2;  // 自动模式
        }
        else {
            isExposureTimeAuto = false;
        }

        qDebug() << isBalanceWhiteAuto << endl;
        int64_t balanceWhiteAutoValue = pIBalanceWhiteAuto->GetIntValue();
        qDebug() << "Balance White Auto Value: " << balanceWhiteAutoValue;

        // 设置白平衡模式的新值
        pIBalanceWhiteAuto->SetIntValue(newBalanceValue);
    }
}

//增益的模式选择
void CameraWorker::onAGCStateChanged(int state)
{
    // 列举增益自动模式
    // Enumeration(pINodeMapRemote, GAIN_AUTO);
    if (IsWritable(pIGainAuto)) {
        int64_t newGainValue = 0;  // 手动模式

        if (state == Qt::Checked) {
            isGainAuto = true;
            newGainValue = 1;  // 自动模式
        }
        else {
            isGainAuto = false;
        }

        qDebug() << isGainAuto << endl;
        int64_t pIGainValue = pIGainAuto->GetIntValue();
        qDebug() << "GainAuto Value: " << pIGainValue;

        // 设置增益模式的新值
        pIGainAuto->SetIntValue(newGainValue);

    }
}

//曝光的模式选择
void CameraWorker::onAEStateChanged(int state)
{
    qDebug() << "onAEStateChanged function is be called" << endl;
    // 列举曝光自动模式
     //Enumeration(pINodeMapRemote, EXPOSURE_AUTO);
    try{
    if (IsWritable(pIExposureAuto)) {
        int64_t newExposureValue = 0;  // 手动模式

        if (state == Qt::Checked) {
            isExposureTimeAuto = true;
            newExposureValue = 1;  // 自动模式
        }
        else {
            isExposureTimeAuto = false;
        }

        qDebug() << isExposureTimeAuto << endl;
        /*int64_t pIExposureModeValue = pIExposureAuto->GetIntValue();
        qDebug() << "ExposureAuto Value: " << pIExposureModeValue;*/

        // 设置曝光模式的新值
        pIExposureAuto->SetIntValue(newExposureValue);
        qDebug() << "ExposureAuto Value: " << newExposureValue;
    }
    }
    catch (const GenICam::GenericException& e) {
        qDebug() << "An exception occurred while setting exposure: " << e.GetDescription() << endl;
    }
}
