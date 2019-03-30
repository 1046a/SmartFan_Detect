#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>


std::map<std::string, std::string> model = {
    {"face", "model/haarcascade_frontalface_alt.xml"},
    {"fullbody", "model/haarcascade_fullbody.xml"},
    {"lowerbody", "model/haarcascade_lowerbody.xml"},
    {"upperbody", "model/haarcascade_upperbody.xml"}
};

std::vector<cv::Rect_<int>> detect(const cv::Mat &img, cv::HOGDescriptor &cls) {
    cv::Mat gray; 
    cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> location;
    std::vector<double> weight;
    cls.detectMultiScale(img, location, weight, 0, cv::Size(16, 16), cv::Size(8, 8), 1.05, 2, false);

    for (size_t i = 0; i < weight.size(); ++i)
        std::cerr << weight[i] << ' ';
    std::cerr << std::endl;

    std::vector<cv::Rect> res;
    for (size_t i = 0; i < weight.size(); ++i) {
        if (weight[i] > 0.8) 
            res.push_back(location[i]);
    }

    return res;
}

template <class Classifier>
std::vector<cv::Rect> detect(const cv::Mat &img, Classifier &cls) {
    cv::Mat gray; 
    cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> res;
    cls.detectMultiScale(gray, res);

    return res;
}

int main() {
    const std::string s = "images/people2.jpg";
    // const std::string v = "images/video.mp4";
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);

    cv::Mat img = cv::imread(s);
    // cv::VideoCapture capture;
    // capture.open(v);

    cv::CascadeClassifier face_detector(model["face"]);
    cv::CascadeClassifier upper_body_detector(model["upperbody"]);
    cv::CascadeClassifier lower_body_detector(model["lowerbody"]);
    cv::CascadeClassifier full_body_detector(model["fullbody"]);
    cv::HOGDescriptor hog();
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
    // cv::HOGDescriptor hog;
    // hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

    std::vector<cv::Rect> upper_bodies = detect(img, upper_body_detector);
    std::vector<cv::Rect> lower_bodies = detect(img, lower_body_detector);
    std::vector<cv::Rect> full_bodies = detect(img, full_body_detector);
    std::vector<cv::Rect> faces = detect(img, face_detector);
    std::vector<cv::Rect> people = detect(img, hog);

    std::cout << (int)faces.size() << " faces found" << std::endl;
    std::cout << (int)upper_bodies.size() << " upper bodies found" << std::endl;
    std::cout << (int)lower_bodies.size() << " lower bodies found" << std::endl;
    std::cout << (int)full_bodies.size() << " full bodies found" << std::endl;
    std::cout << (int)people.size() << " people found" << std::endl;

    for (size_t i = 0; i < faces.size(); ++i) {
        cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        cv::circle(img, center, faces[i].width / 2, cv::Scalar(255, 0, 0), 4);
    }

    // for (size_t i = 0; i < upper_bodies.size(); ++i) 
        // cv::rectangle(img, upper_bodies[i], cv::Scalar(0, 0, 255), 4);

    // for (size_t i = 0; i < lower_bodies.size(); ++i) 
        // cv::rectangle(img, lower_bodies[i], cv::Scalar(255, 255, 255), 4);

    // for (size_t i = 0; i < full_bodies.size(); ++i) 
        // cv::rectangle(img, full_bodies[i], cv::Scalar(255, 255, 255), 4);

    for (size_t i = 0; i < people.size(); ++i) 
        cv::rectangle(img, people[i], cv::Scalar(255, 255, 255), 4);

    cv::imshow("Display window", img);
    cv::waitKey(0);
}
