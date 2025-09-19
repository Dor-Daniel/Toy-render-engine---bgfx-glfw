#include "time.hpp"
#include "../config.hpp"
namespace TimeSystem{
    float time::delta_time = 0.0f;
    float time::cycle_time = 0.0f;
    float time::last       = 0.0f;
    float time::now        = 0.0f;
    class time* time::instance   = nullptr;

    void time::update(){
        now = glfwGetTime();
        delta_time = float(now - last);
        last = now;

        if(delta_time > cycle_time) delta_time = cycle_time; 
    }

    void time::init(){
        cycle_time = 0.1;
        last = now = glfwGetTime();
    }
}

