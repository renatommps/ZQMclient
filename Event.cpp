/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Event.cpp
 * Author: renato
 * 
 * Created on 31 de Outubro de 2016, 15:54
 */

#include "Event.h"

Event::Event(std::string id, std::string video_path, std::string video_name, std::string video_extention, std::time_t start_time) :
_id(id), _videoPath(video_path), _videoName(video_name), _videoExtention(video_extention), _startTime(start_time),
_framesQuantity(0), _horizontalDirection(0), _verticalDirection(0), _motionQuantity(0.0) {
}

Event::Event(const Event& orig) {
}

Event::~Event() {
}

std::string Event::getId() {
    return _id;
}

std::string Event::getVideoPath() {
    return _videoPath;
}

std::string Event::getVideoName() {
    return _videoName;
}

std::string Event::getVideoFullName() {
    return _videoPath + "/" + _videoName + _videoExtention;
}

std::time_t Event::getStartTime() {
    return _startTime;
}

double Event::getDuration() {
    return _duration;
}

long Event::getFramesQuantity() {
    return _framesQuantity;
}

long Event::getHorizontalDirection() {
    return _horizontalDirection;
}

long Event::getVerticalDirection() {
    return _verticalDirection;
}

double Event::getMotionQuantity() {
    return _motionQuantity;
}

void Event::setDuration(double duration) {
    _duration = duration;
}

void Event::setFramesQuantity(long quantity) {
    _framesQuantity = quantity;
}

void Event::setMotionQuantity(double quantity) {
    _motionQuantity = quantity;
}

void Event::incrementFramesQuantity() {
    _framesQuantity++;
}

void Event::incrementMotionQuantity(double quantity) {
    _motionQuantity += quantity;
}

void Event::incrementHorizontalDirection(int quantity) {
    MessageDealer::showMessage("ola to dentro do metodo...");
    _horizontalDirection += quantity;
}

void Event::incrementVerticalDirection(short int quantity) {
    _verticalDirection += quantity;
}