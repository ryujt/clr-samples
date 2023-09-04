#pragma once

#include <windows.h>
#include <stack>

using namespace std;

class PerformanceCounter {
public:
    static PerformanceCounter& getIncetance() {
        static PerformanceCounter instance;
        return instance;
    }

    void enter() {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        _stack.push(counter);
    }

    double leave() {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);

        LARGE_INTEGER exCounter = _stack.top();
        _stack.pop();

        return (counter.QuadPart - exCounter.QuadPart);
    }

    double leaveInMilliseconds() {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);

        LARGE_INTEGER exCounter = _stack.top();
        _stack.pop();

        double elapsedTimeInSeconds = static_cast<double>(counter.QuadPart - exCounter.QuadPart) / _frequency.QuadPart;
        return elapsedTimeInSeconds * 1000.0;
    }

private:
    PerformanceCounter() {
        QueryPerformanceFrequency(&_frequency);
    }

    PerformanceCounter(const PerformanceCounter& ref) {
        QueryPerformanceFrequency(&_frequency);
    }

    PerformanceCounter& operator=(const PerformanceCounter& ref) {}
    ~PerformanceCounter() {}

    LARGE_INTEGER _frequency;
    stack<LARGE_INTEGER> _stack;
};
