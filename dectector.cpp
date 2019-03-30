#include "dectector.hpp"

Dectector::Dectector() {
    _classifier["face"] = cv::CascadeClassifier("model/haarcascade_frontalface_alt.xml");
    _classifier["fullbody"] = cv::CascadeClassifier("model/haarcascade_fullbody.xml");
    _classifier["lowerbody"] = cv::CascadeClassifier("model/haarcascade_lowerbody.xml");
    _classifier["upperbody"] = cv::CascadeClassifier("model/haarcascade_upperbody.xml");
}

std::vector<cv::Rect> Dectector::_detect(const cv::Mat &img, cv::CascadeClassifier &cls) {
    cv::Mat gray; 
    cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> res;
    cls.detectMultiScale(gray, res, 1.15, 3, 0, cv::Size(80, 80), cv::Size());
    return res;
}

std::vector<cv::Rect> Dectector::operator()(const cv::Mat &img) {
    return _detect(img, _classifier["face"]);
}

