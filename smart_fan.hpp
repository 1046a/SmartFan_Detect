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
    std::pair<int, int> detect(int);

private:
    std::pair<int, int> _KNN(std::vector<cv::Rect>);
    cv::VideoCapture _capture;
    Dectector _dectector;
};

extern "C" bool power_up();
extern "C" bool power_off();
extern "C" int detect(int);

#endif
