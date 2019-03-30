#include "opencv2/core/core.hpp"
// #include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>


std::map<std::string, std::string> model = {
    {"face", "model/haarcascade_frontalface_alt.xml"},
    {"body", "model/haarcascade_fullbody.xml"},
    {"lowerbody", "model/haarcascade_lowerbody.xml"},
    {"upperbody", "model/haarcascade_upperbody.xml"}
};

std::vector<cv::Rect_<int>> detect(const cv::Mat &img, cv::CascadeClassifier &cls) {

    cv::Mat original = img.clone();
    cv::Mat gray;
    cvtColor(original, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect_<int>> res;
    cls.detectMultiScale(gray, res, 1.1, 4);

    return res;
}

int main() {
    const std::string s = "images/people2.jpg";
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);

    cv::Mat img = cv::imread(s);

    // if (!img) {
        // std::cerr << "images not found" << std::endl;
        // return -1;
    // }

    cv::CascadeClassifier face_detector(model["face"]);
    cv::CascadeClassifier body_detector(model["upperbody"]);

    std::vector<cv::Rect_<int>> bodies = detect(img, body_detector);
    std::vector<cv::Rect_<int>> faces = detect(img, face_detector);

    std::cout << (int)faces.size() << " faces found" << std::endl;
    std::cout << (int)bodies.size() << " bodies found" << std::endl;

    for (size_t i = 0; i < faces.size(); ++i) {
        cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        cv::circle(img, center, faces[i].width / 2, cv::Scalar(255, 0, 0), 4);
    }

    for (size_t i = 0; i < bodies.size(); ++i) {
        std::cerr << "x = " << bodies[i].x << " y = " << bodies[i].y << " width = " << bodies[i].width << " height = " << bodies[i].height << std::endl;
        cv::rectangle(img, bodies[i], cv::Scalar(0, 0, 255), 4);
    }

    cv::imshow("Display window", img);
    cv::waitKey(0);
}
