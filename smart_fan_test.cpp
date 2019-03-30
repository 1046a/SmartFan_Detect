#include <cassert>
#include <utility>
#include <iostream>

#include "smart_fan.hpp"


int main() {
    SmartFan fan;
    assert(fan.power_up());
    std::pair<int, int> points = fan.detect(50);
    std::cout << points.first << ' ' << points.second << std::endl;
    assert(fan.power_off());
}
