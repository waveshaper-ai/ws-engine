#pragma once

#include "LibAiExportOs.h"
#include "BasicTypes.h"
#include <string>

namespace WS
{
class LIB_AI_EXPORT PipelineTopo
{
public:
    PipelineTopo(size_t frLength);
    PipelineTopo();

    void resetFrameLength(u32 const newFrameLength);

    /// Data members
    u32 frameLength;
    std::string version;
};
} // namespace WS
