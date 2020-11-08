#pragma once

#include <chrono>

class Timer{
    Timer(){
        startTime = std::chrono::steady_clock::now();
    }

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currTime;
};
