#include <stdlib.h>         // atoi
#include <thread>           // std::thread
#include <string>           // std::string
#include <fstream>          // std::fstream
#include <fstream>          // std::ofstream
#include <list>             // std::list
#include <unistd.h>         // std::usleep
#include <chrono>           // std::chrono::milliseconds
#include <mutex>               // std::mutex, std::unique_lock
#include <condition_variable>  // std::condition_variable
#include <opencv2/opencv.hpp>
#include <zmq.hpp>

#include "zhelpers.hpp"
#include "BufferManager.h"
#include "SynchronizationAndStatusDealer.h"
#include "MessageDealer.h"
#include "ProcessingTask.h"
#include "StorageTask.h"
#include "ClientTask.h"
#include "Event.h"
#include "Frame.h"

std::string getExecutionPath();
bool createDirectoryTree(std::string path);
bool createDirectory(std::string sub_path);
bool setClientName(std::string * c_name);
void readInicialFrame(cv::VideoCapture * cap_device, cv::Mat * f, cv::Mat * pf, cv::Mat * ppf);
void setFrameParameters(cv::Mat * f, int * f_width, int * f_height, int * pf_width, int * pf_height, int * max_mov_width, int * min_mov_width,
        int * min_horizontal_mov_dist, int * min_vertical_mov_dist, int * pf_border_width);
void defineServoPosition(short int servo_channel, short int position);
static bool ProcessingTaskInNormalFunction();

cv::VideoCapture _cap;
cv::Mat _frame;
cv::Mat _processedFrame;
cv::Mat _clientFrame;
cv::Mat _LastProcessedFrame;
std::list<Frame> frame_buffer;
std::list<Event*> _eventsList;

int _frameWidth;
int _frameHeight;
int _processedFrameWidth;
int _processedFrameHeight;
int _processedFrameBorderWidth;
int _maximum_movement_width;
int _minimal_movement_width;
int _minimalHorizontalMomementDistance;
int _minimalVerticalMomementDistance;

bool errorOnProcessingTask = false;
bool ProcessingTaskDone = false;
bool errorOnClientTask = false;
bool clientThreadNotified = false;
bool clientTaskInitializad = false;

std::condition_variable cond_var;
std::mutex variable_access_mutex;
std::mutex sincronization_mutex;
std::mutex clientFrame_mutex;
std::mutex buffer_mutex;

int main(int argc, char * argv[]) {

    std::string _executablePath;
    std::string _eventsStoragePath;
    int _capDeviceIndex;
    std::string _serverIP;
    std::string _serverPort; // = "5570";
    std::string _mac;
    bool _horizontal_tracking;
    bool _vertical_tracking;
    bool _show_motion;

    /* É esperado 6 argumentos: o nome do programa (por padrão é passado),
     * o índice do dispositivo de vídeo, o IP do server, a porta do server,
     * um número indicando se deve ser feito tracking horizontal e vertical
     * com o servo, são dois números separados por espaço, 0 indicando que não deve ser feito,
     * e 1 undicando que deve, para tracking horizontal e vertical, respectivamente */
    if (argc < 7) {
        MessageDealer::showErrorMessage("Usage: " + std::string(argv[0]) +
                " <VIDEO DEVICE INDEX> <SERVER IP> <SERVER PORT> "
                "<HORIZONTAL TRACKING [0 = false, 1 = true]> "
                "<VERTICAL TRACKING [0 = false, 1 = true]>"
                "<SHOW MOTION [0 = false, 1 = true]>");
        exit(1);

    } else {
        _executablePath = getExecutionPath();
        _eventsStoragePath = _executablePath + std::string("/eventsStorage");
        _capDeviceIndex = atoi(argv[1]);
        _serverIP = argv[2];
        _serverPort = argv[3];
        (std::strcmp(argv[4], "1") == 0) ? _horizontal_tracking = true : _horizontal_tracking = false;
        (std::strcmp(argv[5], "1") == 0) ? _vertical_tracking = true : _vertical_tracking = false;
        (std::strcmp(argv[6], "1") == 0) ? _show_motion = true : _show_motion = false;

        MessageDealer::showMessage(
                "\nExecutablePath: " + _executablePath +
                "\nCapture device index: " + std::to_string(_capDeviceIndex) +
                "\nServer IP: " + _serverIP +
                "\nServer port: " + _serverPort +
                "\nHorizontal tracking enabled: " + (_horizontal_tracking == 1 ? "true" : "false") +
                "\nVertical tracking enabled: " + (_vertical_tracking == 1 ? "true" : "false") +
                "\n");
    }

    if (createDirectoryTree(_eventsStoragePath)) {
        MessageDealer::showErrorMessage("Events Storage directory created. Directory path: " + _eventsStoragePath);
    }

    if (!setClientName(&_mac)) {
        MessageDealer::showErrorMessage("MAC file doesn't find, it wasn't possible to define client name");
        exit(1);
    }

    //int capDeviceIndex;
    BufferManager frameBuffer;
    SynchronizationAndStatusDealer synchAndStatusDealer;

    ProcessingTask pt(_eventsStoragePath, _capDeviceIndex, _horizontal_tracking, _vertical_tracking, &_eventsList, &frameBuffer, &synchAndStatusDealer, _show_motion);
    StorageTask st(&frameBuffer, &synchAndStatusDealer);
    ClientTask ct(_mac, _serverIP, _serverPort, &frameBuffer, &synchAndStatusDealer);

    std::thread tp(&ProcessingTask::start, &pt);
    std::thread ts(&StorageTask::start, &st);
    std::thread tc(&ClientTask::start, &ct);

    MessageDealer::showMessage("Main execution will join tasks and wait to finish it's execution.");
    tp.join();
    ts.join();
    tc.join();
    MessageDealer::showMessage("Main execution finished, process terminated!");

    return 0;
}

std::string getExecutionPath() {
    std::string path = "";
    int path_max = 1024;
    char buff[path_max];
    ssize_t len = readlink("/proc/self/exe", buff, sizeof ( buff) - 1);

    if (len != -1) {
        buff[len] = '\0';
        size_t found;
        std::string full_name = std::string(buff);
        found = full_name.find_last_of("/\\");
        path = full_name.substr(0, found);
    }
    return path;
}

bool createDirectoryTree(std::string path) {
    size_t position = 0;
    bool return_status = true;

    while (return_status == true && position != std::string::npos) {
        position = path.find('/', position + 1);
        return_status = createDirectory(path.substr(0, position));
    }
    return return_status;
}

bool createDirectory(std::string sub_path) {
    mkdir(sub_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return true;
}

bool setClientName(std::string * c_name) {
    bool client_set = false;
    std::ifstream infile("mac.txt");

    if (infile.good()) {
        std::getline(infile, *c_name);
        client_set = true;
    } else {
        MessageDealer::showErrorMessage("Could'not read mac.txt file");
    }
    infile.close();

    return client_set;
}

void readInicialFrame(cv::VideoCapture * cap_device, cv::Mat * f, cv::Mat * pf, cv::Mat * ppf) {
    cap_device->read(*f);
    cv::resize(*f, *pf, DEFAULT_PROCESSED_FRAME_SIZE, 0, 0, CV_INTER_AREA);
    *ppf = pf->clone();
}

void setFrameParameters(cv::Mat * f, int * f_width, int * f_height, int * pf_width, int * pf_height, int * max_mov_width,
        int * min_mov_width, int * min_horizontal_mov_dist, int * min_vertical_mov_dist, int * pf_border_width) {
    *f_width = f->cols;
    *f_height = f->rows;

    *pf_width = *f_width / DEFAULT_RESIZE_SCALE;
    *pf_height = *f_height / DEFAULT_RESIZE_SCALE;

    *pf_border_width = (int) * pf_width / 4;

    *max_mov_width = (*pf_width / 2.5); // 40% do tamanho total da imagem (tamanho/100)*40 = tamanho / 2.5
    *min_mov_width = (*pf_width / 20); // 5% do tamanho total da imagem (tamanho/100)*5 = tamanho / 20

    *min_horizontal_mov_dist = *pf_width / 10; // a distancia horizontal percorrida entre detecções deve ser de no mínimo 10% do tamanho horzontal da imagem processada
    *min_vertical_mov_dist = *pf_height / 10; // a distancia vertical percorrida entre detecções deve ser de no mínimo 10% do tamanho vertical da imagem processada

    std::cout << "Processed Frame size = " << *pf_width << "x" << *pf_height << ", area = " << (*pf_width) * (*pf_height) << std::endl;
}