#pragma once

#include "BasicTypes.h"
#include "StrFormat.h"
#include <chrono>
#include <iostream>

namespace WS
{
namespace Util
{
/// Useful high-resolution timer simple to use. Is based on std::chrono.
class Timer
{
public:
    Timer() = default;
    virtual ~Timer() = default;

    inline void start()
    {
        mTotalTime = std::chrono::milliseconds{0};
        mT1 = std::chrono::high_resolution_clock::now();
    }

    inline std::chrono::milliseconds elapsed(std::string const& message)
    {
        mT2 = std::chrono::high_resolution_clock::now();
        std::chrono::milliseconds elapsed{std::chrono::duration_cast<std::chrono::milliseconds>(mT2 - mT1).count()};
        printMessage(message, elapsed);
        return elapsed;
    }

    inline std::chrono::milliseconds stop(std::string const& totalTimeSinceStartMessage)
    {
        mT2 = std::chrono::high_resolution_clock::now();
        std::chrono::milliseconds total{std::chrono::duration_cast<std::chrono::milliseconds>(mT2 - mT1).count()};
        mTotalTime += total;
        std::cout << WS::Util::Format::formatStr("{} took {}.\n", totalTimeSinceStartMessage, mTotalTime);
        return mTotalTime;
    }

private:
    inline void printMessage(std::string const& message, std::chrono::milliseconds elapsed)
    {
        std::cout << WS::Util::Format::formatStr("{} took {}.\n", message, elapsed);
    }

    // Data members
private:
    std::chrono::high_resolution_clock::time_point mT1{};
    std::chrono::high_resolution_clock::time_point mT2{};
    std::chrono::milliseconds mTotalTime{0};
};
} // namespace Util
} // namespace WS
