/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Event.h
 * Author: renato
 *
 * Created on 31 de Outubro de 2016, 15:54
 */

#ifndef EVENT_H
#define EVENT_H

#include <ctime>        //std::time_t
#include <time.h>       //time_t, time, ctime
#include <string>       //std::string
#include <opencv2/opencv.hpp>
#include "opencv/highgui.h"
#include "MessageDealer.h"

const int VIDEO_WRITER_CODEC = CV_FOURCC('M', 'J', 'P', 'G'); // {'F','M','P','4'}; // codec utilizado pelo VideoWriter

class Event {
public:
    Event(std::string id, std::string video_path, std::string video_name, std::string video_extention, std::time_t start_time, double fps, cv::Size size);
    Event(const Event& orig);
    virtual ~Event();

    std::string getId();
    std::string getVideoPath();
    std::string getVideoName();
    std::string getVideoFullName();
    std::time_t getStartTime();
    double getDuration();
    long getFramesQuantity();
    long getHorizontalDirection();
    long getVerticalDirection();
    double getMotionQuantity();
    bool getEventSavedStatus();

    void setDuration(double duration); // define a duração em segundos do evento
    void setFramesQuantity(long quantity); // define a quantidade acumulada de frames (quadros de vídeo) do evento
    void setMotionQuantity(double quantity); // define a quantidade de movimentação acumulada do evento
    void setEventSavedStatus(bool status); // seta se o evento foi enviado com sucesso para o servidor
    void incrementFramesQuantity(); // incrementa de um a quantidade acumulada de frames
    void incrementMotionQuantity(double quantity); // incrementa a quantidade acumulada de movimentação do evento com a quantidade passada
    void incrementHorizontalDirection(int quantity); // incrementar de 1 significa incremento para a direita, e -1, para a esquerda;
    void incrementVerticalDirection(short int quantity); // incrementar de 1 significa incremento para cima, e -1, para baixo;
    std::string getFormatedTime(std::time_t raw_time, std::string format);
    bool saveInfoOnDisk(); // salva os dados do evento em disco para ser recuperado posteriormente
    std::string toString();
private:
    std::string _id; // identificação do evento
    std::string _videoPath; // caminho (do diretório/pasta) do vídeo que sera gravado do evento
    std::string _videoName;
    std::string _videoExtention; // extensão usada para salvar o vídeo do evento (Ex: .mp4, .avi, etc...)
    std::time_t _startTime;
    double _duration; // duração do evento em segundos;
    long _framesQuantity; // quantidade acumulada de frames (quadros de vídeo) do evento
    int _horizontalDirection; // valores negativos representam direção média de movimentos para a esquerda, e positivos para a direita;
    long _verticalDirection; // valores negativos representam direção média de movimentos para baixo, e positivos para cima;
    double _motionQuantity; // quantia acumulada de movimentação detectada do evento
    bool _eventSavedStatus; // indica se o evento já foi enviado e salvo no servidor
    cv::VideoWriter _videoWriter;
    cv::Size _videoSize;
    double _fps;
};

#endif /* EVENT_H */

