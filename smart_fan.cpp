#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <random>
#include <iostream>
#include <ctime>

#include "smart_fan.hpp"


SmartFan::SmartFan(): _state(false), _iter(10), _idleness(5) {}

bool SmartFan::power_up() {
    _capture.open(0); 
    return _capture.isOpened();
}

bool SmartFan::power_off() {
    _capture.release();
    return true;
}

std::pair<int, int> SmartFan::_detect() {
    assert(_capture.isOpened());
    cv::Mat frame;
    std::vector<cv::Rect> rects;

    // _capture.open(0);
    // cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    clock_t before = clock();

    int iter = _iter;
    while (iter-- && _capture.read(frame)) {

        std::vector<cv::Rect> location = _dectector(frame);
        rects.insert(rects.end(), location.begin(), location.end());

        // for (size_t i = 0; i < location.size(); ++i)
            // std::cout << location[i].x << ' ' << location[i].y << ' ' << location[i].width << ' ' << location[i].height << std::endl;

        for (size_t i = 0; i < location.size(); ++i) {
            // cv::Point p(location[i].x + location[i].width / 2, location[i].y + location[i].height / 2);
            // cv::circle(frame, p, location[i].width / 2, cv::Scalar(255, 0, 0), 4);
            // cv::rectangle(frame, location[i], cv::Scalar(255, 0, 0), 4);
        }
        // cv::imshow("Display window", frame);
        // if (cv::waitKey(10) == 27)
            // break;
        // std::cerr << "Iter = " << iter << std::endl;
    }

    // std::cerr << "Time elapsed: " << 1.0 * (clock() - before) / CLOCKS_PER_SEC << std::endl;

    std::pair<int, int> res = _KNN(rects);
    // std::cout << "x = " << res.first << " y = " << res.second << std::endl;

    // _capture.release();
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
        return abs(a.first - (b.x + b.width / 2));
    };

    std::mt19937 rng(7122);
    std::uniform_int_distribution<int> dis(0, (int)points.size() - 1);

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
                npivot[belong[i]].first += points[i].x + points[i].width / 2;
                npivot[belong[i]].second += points[i].y + points[i].height / 2;
                ++members[belong[i]];
            }

            pbelong = belong;

            for (size_t i = 0; i < k; ++i) {
                if (members[i] == 0) {
                    std::cerr << "[Warning] KNN has group with 0 size" << std::endl;
                    int j = dis(rng);
                    npivot[i].first = points[j].x + points[j].width / 2;
                    npivot[i].second = points[j].y + points[j].height / 2;
                    continue;
                }
                npivot[i].first /= members[i];
                npivot[i].second /= members[i];
            }

            pivot = npivot;
        }
        
        std::vector<std::vector<cv::Rect>> nodes(k);
        for (size_t i = 0; i < points.size(); ++i)
            nodes[pbelong[i]].push_back(points[i]);

        bool feasible = true;

        std::cerr << "[DEBUG] KNN result when k = " << k << std::endl;
        for (size_t i = 0; i < k; ++i) {
            int x_max = -1e9, x_min = 1e9;
            for (size_t j = 0; j < nodes[i].size(); ++j) {
                x_max = std::max(x_max, nodes[i][j].x);
                x_min = std::min(x_min, nodes[i][j].x);
                std::cerr << nodes[i][j].x << ' ';
            }
            std::cerr << std::endl;
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

    __builtin_unreachable();
}

int SmartFan::state(double *alpha) {
    std::pair<int, int> position = _detect();    
    // std::cout << "x = " << position.first << " y = " << position.second << std::endl;

    if (position.first == 7122) {
        if (!_state) 
            return 3;
        if (++_idle == _idleness) {
            _idle = 0;
            _state = false; 
            return 0;
        }
        *alpha = 0.0;
        return 2;
    } else {
        _idle = 0;
        if (!_state) {
            _state = true;
            *alpha = 0.0;
            return 1;
        } else {
            int d = position.first - 512;
            *alpha = _get_angle(d);
            return 2;
        }
    }

    __builtin_unreachable();
}

double SmartFan::_get_angle(int d) {
    std::cout << "d = " << d << " angle = " << atan2(512 / tan(_theta / 2), d) << std::endl;
    return atan2(512 / tan(_theta / 2), d);
}

void SmartFan::set_iter(int iter) { _iter = iter; }
void SmartFan::set_idleness(int idleness) { _idleness = idleness; }
void SmartFan::set_theta(double theta) { _theta = theta; }

SmartFan _fan;

bool power_up() {
    return _fan.power_up();
}

bool power_off() {
    return _fan.power_off();
}

int state(double *a) {
    return _fan.state(a);
}

void set_iter(int iter) {
    _fan.set_iter(iter);
}

void set_idleness(int idleness) {
    _fan.set_idleness(idleness);
}

void set_theta(double theta) {
    _fan.set_theta(theta);
}
