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

std::pair<int, int> SmartFan::detect(int iter = 10) {
    assert(_capture.isOpened());
    cv::Mat frame;
    std::vector<cv::Rect> rects;

    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    clock_t before = clock();
    while (iter-- && _capture.read(frame)) {

        std::vector<cv::Rect> location = _dectector(frame);
        rects.insert(rects.end(), location.begin(), location.end());

        for (size_t i = 0; i < location.size(); ++i)
            std::cout << location[i].x << ' ' << location[i].y << ' ' << location[i].width << ' ' << location[i].height << std::endl;

        for (size_t i = 0; i < location.size(); ++i) {
            // cv::Point p(location[i].x + location[i].width / 2, location[i].y + location[i].height / 2);
            // cv::circle(frame, p, location[i].width / 2, cv::Scalar(255, 0, 0), 4);
            cv::rectangle(frame, location[i], cv::Scalar(255, 0, 0), 4);
        }
        cv::imshow("Display window", frame);
        if (cv::waitKey(10) == 27)
            break;
        // std::cerr << "Iter = " << iter << std::endl;
    }

    // std::cerr << "Time elapsed: " << 1.0 * (clock() - before) / CLOCKS_PER_SEC << std::endl;

    std::pair<int, int> res = _KNN(rects);
    // std::cout << "x = " << res.first << " y = " << res.second << std::endl;

    return res;
}

std::pair<int, int> SmartFan::_KNN(std::vector<cv::Rect> points) {
    if (points.empty())
        return std::make_pair(7122, 7122);

    int x = 0, y = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        x += points[i].x;
        y += points[i].y;
    }

    auto dist = [&](std::pair<double, double> a, cv::Rect b) -> double {
        return hypot(a.first - b.x, a.second - b.y);
    };

    std::mt19937 rng(7122);

    for (int k = 1; k <= (int)points.size(); ++k) {
        std::shuffle(points.begin(), points.end(), rng);
        std::vector<std::pair<double, double>> pivot;
        for (size_t j = 0; j < k; ++j)
            pivot.emplace_back(points[j].x, points[j].y);

        std::vector<int> pbelong(points.size(), -1);

        while (true) {
            std::vector<int> belong(points.size(), -1);
            bool diff = false;
            for (size_t i = 0; i < points.size(); ++i) {
                double mdist = 1e9;
                for (size_t j = 0; j < pivot.size(); ++j) {
                    if (mdist > dist(pivot[j], points[i])) {
                        mdist = dist(pivot[j], points[i]);
                        belong[i] = j;
                    }
                }
                diff |= belong[i] != pbelong[i];
            }

            if (!diff) break;

            std::vector<int> members(k);
            std::vector<std::pair<double, double>> npivot(k);
            for (size_t i = 0; i < points.size(); ++i) {
                npivot[belong[i]].first += points[i].x;
                npivot[belong[i]].second += points[i].y;
                ++members[belong[i]];
            }

            pbelong = belong;

            for (size_t i = 0; i < k; ++i) {
                assert(members[i] > 0);
                npivot[i].first /= members[i];
                npivot[i].second /= members[i];
            }

            pivot = npivot;
        }
        
        std::vector<std::vector<cv::Rect>> nodes(k);
        for (size_t i = 0; i < points.size(); ++i)
            nodes[pbelong[i]].push_back(points[i]);

        bool feasible = true;

        for (size_t i = 0; i < k; ++i) {
            int x_max = -1e9, x_min = 1e9;
            for (size_t j = 0; j < nodes[i].size(); ++j) {
                x_max = std::max(x_max, nodes[i][j].x);
                x_min = std::min(x_min, nodes[i][j].x);
            }
            feasible &= x_max - x_min <= 70;
        }

        if (feasible) {
            int res = 0;
            for (size_t i = 0; i < k; ++i) {
                if (nodes[i].size() > nodes[res].size())
                    res = i;
            }

            int x = std::accumulate(nodes[res].begin(), nodes[res].end(), 0, [](int a, cv::Rect b) {
                return a + b.x;
            });
            int y = std::accumulate(nodes[res].begin(), nodes[res].end(), 0, [](int a, cv::Rect b) {
                return a + b.y;
            });

            std::cerr << "KNN find feasible solution with k = " << k << std::endl;

            return std::make_pair(x / (int)nodes[res].size(), y / (int)nodes[res].size());
        }
    }

    throw;
    // return std::make_pair(x / (int)points.size(), y / (int)points.size());
}

SmartFan _fan;

bool power_up() {
    return _fan.power_up();
}

bool power_off() {
    return _fan.power_off();
}

int detect(int iter = 10) {
    return _fan.detect(iter).first;
}
