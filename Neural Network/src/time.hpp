#pragma once

#include <chrono>
#include <thread>

namespace Time
{
    struct Timer
    {
        Timer() :
            start(std::chrono::high_resolution_clock::now()) {}

        auto ElapsedMicro()
        {
            return (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - this->start)).count();
        }
        auto ElapsedMillis()
        {
            return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - this->start)).count();
        }
        auto ElapsedSec()
        {
            return (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - this->start)).count();
        }
        auto ElapsedMin()
        {
            return (std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - this->start)).count();
        }
        auto ElapsedHour()
        {
            return (std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - this->start)).count();
        }
        void Reset() { start = std::chrono::high_resolution_clock::now(); }

        std::chrono::time_point<std::chrono::high_resolution_clock> start;
    };

    inline Timer now()
    {
        return Timer();
    }
    inline void wait(uint32_t time)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
}