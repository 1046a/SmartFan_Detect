#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <random>

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

std::pair<double, double> SmartFan::detect(int iter) {
    cv::Mat frame;
    std::vector<std::pair<double, double>> points;
    std::vector<cv::Rect> buffer;

    while (iter-- && _capture.read(frame)) {
        std::vector<cv::Rect> location = _dectector(frame);
        buffer.insert(buffer.end(), location.begin(), location.end());
        for (size_t i = 0; i < location.size(); ++i)
            points.emplace_back(location[i].x, location[i].y);
    }

    std::vector<std::pair<double, double>> centers = _KNN(points);
}

std::vector<std::pair<double, double>> SmartFan::_KNN(std::vector<std::pair<double, double>> points) {
    
    std::mt19937 rng(7122);

    auto dist = [&](const std::pair<double, double> &a, const std::pair<double, double> &b) -> double {
        return hypot(a.first - b.first, a.second - b.second);
    };

    for (int k = 1; k <= 2; ++k) {
        std::shuffle(points.begin(), points.end(), rng);
        std::vector<std::pair<double, double>> centers(points.begin(), points.begin() + k);

        while (true) {
            std::vector<int> choose(points.size(), -1);
            for (size_t i = 0; i < points.size(); ++i) {
                double mdist = 1e9;
                for (size_t j = 0; j < k; ++j) {
                    if (dist(points[i], centers[j]) < mdist) {
                        mdist = dist(points[i], centers[j]);
                        choose[i] = j;
                    }
                }
            }

            std::vector<std::pair<double, double>> ncenters(k);
            std::vector<int> counts(k);
            for (size_t i = 0; i < points.size(); ++i) {
                ncenters[choose[i]].first += points[i].first;
                ncenters[choose[i]].second += points[i].second;
            }
            bool diff = false;
            for (size_t i = 0; i < k; ++i) {
                assert(counts[i] > 0);
                ncenters[i].first /= counts[i];
                ncenters[i].second /= counts[i];

                diff |= fabs(ncenters[i].first - centers[i].first) > 1e-7 ||
                        fabs(ncenters[i].second - centers[i].second) > 1e-7;
            }

            if (!diff)
                break;

        }
    }
}
