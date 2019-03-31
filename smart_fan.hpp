#ifndef SMART_FAN_HPP_INCLUDED
#define SMART_FAN_HPP_INCLUDED

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <utility>
#include <vector>

#include "dectector.hpp"


class SmartFan {
public:
    SmartFan();
    bool power_up();
    bool power_off();
    int state(double*);
    void set_iter(int);
    void set_idleness(int);
    void set_theta(double);

private:
    bool _state;
    int _idle, _iter, _idleness, _width;
    double _theta;
    double _get_angle(int);
    std::pair<int, int> _detect();
    std::pair<int, int> _KNN(std::vector<cv::Rect>);
    cv::VideoCapture _capture;
    Dectector _dectector;
};


extern "C" bool power_up();
extern "C" bool power_off();
extern "C" int state(double*);
extern "C" void set_iter(int);
extern "C" void set_idleness(int);
extern "C" void set_theta(double);

#endif
