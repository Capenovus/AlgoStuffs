#pragma once
#include "imports.h"

std::string readfile(std::string file, bool toHex = false);
int supress_stdout();
void resume_stdout(int fd);

template <class DT = std::chrono::seconds,
    class ClockT = std::chrono::steady_clock>
class Timer
{
    using timep_t = typename ClockT::time_point;
    timep_t _start = ClockT::now(), _end = {};

public:
    void tick() {
        _end = timep_t{};
        _start = ClockT::now();
    }

    void tock() { _end = ClockT::now(); }

    template <class T = DT>
    auto duration() const {
        //gsl_Expects(_end != timep_t{} && "toc before reporting");
        return std::chrono::duration_cast<T>(_end - _start);
    }
};