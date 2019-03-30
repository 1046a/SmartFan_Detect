#ifndef DECTECTOR_HPP_INCLUDED
#define DECTECTOR_HPP_INCLUDED

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <map>
#include <string>
#include <vector>

class Dectector {
private:
    std::map<std::string, cv::CascadeClassifier> _classifier;
    std::vector<cv::Rect> _detect(const cv::Mat &, cv::CascadeClassifier &);

public:
    Dectector();
    std::vector<cv::Rect> operator()(const cv::Mat &);
};

#endif
