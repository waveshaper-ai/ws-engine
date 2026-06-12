#pragma once

#include <string>

namespace WS
{
class Logger
{
public:
    Logger() = default;
    virtual ~Logger() = default;


    virtual void logMessage(std::string const& message) = 0;
};
}