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
    std::pair<double, double> detect(int);

private:
    std::vector<std::pair<double, double>> _KNN(const std::vector<std::pair<double, double>>);
    cv::VideoCapture _capture;
    Dectector _dectector;
};
