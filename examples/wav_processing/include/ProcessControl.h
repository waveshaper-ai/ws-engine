#pragma once

#include "LibUtilExportOs.h"

namespace WS
{
namespace Util
{
/// ParamContext holds input parameters to the processing as well
/// as different context necessary (e.g. GPU OpenCL, OpenGL, etc)
class LIBUTIL_EXPORT ProcessControl
{
    bool mAbortProcessing{false};

public:
    ProcessControl() = default;
    virtual ~ProcessControl() = default;

    /// Operations on the class

    /// Call from a thread different than the processing thread.
    /// Tells the process thread processing needs to terminate,
    /// as the processing is aborted.
    void abortProcessing();

    // Accessors
    bool mustAbortProcessing() const;

private:
};

} // namespace Util
} // namespace WS
