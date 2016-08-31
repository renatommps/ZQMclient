/* 
 * File:   processing_task.h
 * Author: renato
 *
 * Created on 30 de Agosto de 2016, 15:00
 */

#ifndef PROCESSINGTASK_H
#define PROCESSINGTASK_H

#include <string>       //std::string
#include "messageDealer.h"
#include "bufferManager.h"
#include "synchronizationAndStatusDealer.h"

const short int DEFAULT_WIDTH = 640; // width (largura) padão do frame
const short int DEFAULT_HEIGHT = 480; // height (altura) padão do frame
const short int DEFAULT_RESIZE_SCALE = 4; // fator usado para redimensionar o frame que sera processado (para aumentar a velocidade de processamento)
const cv::Size DEFAULT_PROCESSED_FRAME_SIZE(DEFAULT_WIDTH / DEFAULT_RESIZE_SCALE, DEFAULT_HEIGHT / DEFAULT_RESIZE_SCALE); // tamanho do frame que sera processado

class processingTask {
public:
    processingTask();
    processingTask(int capDeviceIndex, bufferManager *buffer, synchronizationAndStatusDealer *synchAndStatusDealer);
    virtual ~processingTask();
    void start();
private:
    bool _executionError;
    int _capDeviceIndex;
    cv::VideoCapture _cap;
    cv::Mat _frame;
    cv::Mat _processedFrame;
    cv::Mat _LastProcessedFrame;
    bufferManager *_frameBuffer;
    synchronizationAndStatusDealer *_synchAndStatusDealer;
    
    bool openAndConfigureVideoDevice();
};

#endif /* PROCESSINGTASK_H */
