#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <random>
#include <iostream>
#include <ctime>

#include "smart_fan.hpp"


SmartFan::SmartFan() {

}

bool SmartFan::power_up() {
    _capture.open(0); 
    return _capture.isOpened();
}

bool SmartFan::power_off() {
    _capture.release();
    return true;
}

std::pair<int, int> SmartFan::detect(int iter) {
    assert(_capture.isOpened());
    cv::Mat frame;
    std::vector<cv::Rect> rects;

    // cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    clock_t before = clock();
    while (iter-- && _capture.read(frame)) {

        std::vector<cv::Rect> location = _dectector(frame);
        rects.insert(rects.end(), location.begin(), location.end());

        // for (size_t i = 0; i < location.size(); ++i) {
            // cv::Point p(location[i].x + location[i].width / 2, location[i].y + location[i].height / 2);
            // cv::circle(frame, p, location[i].width / 2, cv::Scalar(255, 0, 0), 4);
        // }
        // cv::imshow("Display window", frame);
        // if (cv::waitKey(10) == 27)
            // break;
    }

    std::cerr << "Time elapsed: " << 1.0 * (clock() - before) / CLOCKS_PER_SEC << std::endl;

    return _KNN(rects);
}

std::pair<int, int> SmartFan::_KNN(std::vector<cv::Rect> points) {
    std::cerr << "points.size() = " << (int)points.size() << std::endl;
    std::mt19937 rng(7122);

    auto dist = [&](std::pair<double, double> a, cv::Rect b) -> double {
        return hypot(a.first - b.x, a.second - b.y);
    };

    for (int k = 1; k <= 2; ++k) {
        std::shuffle(points.begin(), points.end(), rng);
        std::vector<std::pair<double, double>> centers;

        for (size_t i = 0; i < k; ++i)
            centers.emplace_back(points[i].x, points[i].y);

        while (true) {
            std::vector<int> choose(points.size(), -1);
            for (size_t i = 0; i < points.size(); ++i) {
                double mdist = 1e9;
                for (size_t j = 0; j < k; ++j) {
                    if (dist(centers[j], points[i]) < mdist) {
                        mdist = dist(centers[j], points[i]);
                        choose[i] = j;
                    }
                }
            }

            std::vector<std::pair<double, double>> ncenters(k);
            std::vector<int> counts(k);
            for (size_t i = 0; i < points.size(); ++i) {
                ncenters[choose[i]].first += points[i].x;
                ncenters[choose[i]].second += points[i].y;
                ++counts[choose[i]];
            }
            bool diff = false;
            for (size_t i = 0; i < k; ++i) {
                if (counts[i] == 0) {
                    std::cerr << "failed when k = " << k << std::endl;
                    throw;
                }
                ncenters[i].first /= counts[i];
                ncenters[i].second /= counts[i];

                // std::cerr << "center = " << centers[i].first << ' ' << centers[i].second << std::endl;
                // std::cerr << "ncenter = " << ncenters[i].first << ' ' << ncenters[i].second << std::endl;

                diff |= fabs(ncenters[i].first - centers[i].first) > 1e-7 ||
                        fabs(ncenters[i].second - centers[i].second) > 1e-7;
            }

            if (!diff)
                break;
        }
    }
    return std::make_pair(1, 1);
}
