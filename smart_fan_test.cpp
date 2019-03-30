#include <cassert>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <utility>

#include "smart_fan.hpp"


int main() {
    SmartFan fan;
    fan.set_iter(10);
    fan.set_theta(0.25 * acos(-1));
    fan.set_idleness(5);
    assert(fan.power_up());
    while (true) {
        double *alpha = new double();
        int state = fan.state(alpha);
        if (state == 0)
            break;
        std::cout << "state = " << state << std::endl;
        std::cout << "alpha = " << *alpha << std::endl;
        // sleep(0.3);
    }
    assert(fan.power_off());
}
