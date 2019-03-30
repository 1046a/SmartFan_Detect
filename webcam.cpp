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

template <class Classifier>
std::vector<cv::Rect> detect(const cv::Mat &img, Classifier &cls) {

    cv::Mat gray; 
    cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> res;
    cls.detectMultiScale(gray, res);

    return res;
}

int main() {
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) {
        std::cout << "no webcam" << std::endl;
        return 0;
    }

    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::CascadeClassifier face_detector(model["face"]);

    cv::Mat frame;

    while (capture.read(frame)) {
        if (frame.empty())
            break;

        // // std::vector<cv::Rect> people = detect(frame, hog);
        // std::vector<cv::Rect> body = detect(frame, body_detector);
        std::vector<cv::Rect> face = detect(frame, face_detector);

        // // for (size_t i = 0; i < people.size(); ++i) {
            // // cv::rectangle(frame, people[i], cv::Scalar(255, 0, 0), 4);
            // // cv::Point p(face[i].x + face[i].width / 2, face[i].y + face[i].height / 2);
            // // cv::circle(frame, p, face[i].width / 2, cv::Scalar(255, 0, 0), 4);
        // // }
        // for (size_t i = 0; i < body.size(); ++i) {
            // cv::rectangle(frame, body[i], cv::Scalar(0, 255, 0), 4);
            // // cv::Point p(face[i].x + face[i].width / 2, face[i].y + face[i].height / 2);
            // // cv::circle(frame, p, face[i].width / 2, cv::Scalar(255, 0, 0), 4);
        // }

        for (size_t i = 0; i < face.size(); ++i) {
            cv::Point p(face[i].x + face[i].width / 2, face[i].y + face[i].height / 2);
            cv::circle(frame, p, face[i].width / 2, cv::Scalar(255, 0, 0), 4);
        }

        cv::imshow("Display window", frame);

        if (cv::waitKey(10) == 27)
            break;
    }
}
