#pragma once

#include "CmdLineParser.h"
#include <string>

namespace WS
{
/// @brief  Class StremManager is used as the class that handles reaind portion of the file, process portion the file
/// and write the result file in outWacPathName.
class StreamManager
{
public:
    // Returns the version of this software
    static std::string getVersion();

    /// @brief  New entry point which re-use the lib-streamer code.
    static int processFile(TL::LibCore::CmdLineParser& parser);
};

} // namespace WS