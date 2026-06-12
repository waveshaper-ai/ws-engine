#pragma once

#include "BasicTypes.h"
#include "OpenALContext.h"
#include "TimeFrame.h"

namespace WS
{
namespace Util
{
class ProcessControl;

/// ParamContext holds input parameters to the processing as well
/// as different context necessary (e.g. GPU OpenCL, OpenGL, etc)
class LIBUTIL_EXPORT ParamContext
{
public:
    ParamContext();
    ParamContext(u32 width, u32 height);
    ParamContext(TimeFrame const& globalTime, OpenALContext* oalContext);
    ParamContext(ProcessControl& procControl);
    virtual ~ParamContext() = default;

    /// Operations on the class
    bool mustAbortProcessing() const;

    /// Accessors
    inline TimeFrame const& getGlobalTime() const { return mGlobalTime; }
    inline OpenALContext* getOpenALContext() { return mOpenALContext; }
    inline u32 getTargetWidth() { return mWidth; }
    inline u32 getTargetHeight() { return mHeight; }
    inline void setTargetSize(u32 width, u32 height)
    {
        mWidth = width;
        mHeight = height;
    }
    inline void setComputeOption(u32 option)
    {
        mComputeOption = option;
    }
    inline u32 getComputeOption() const { return mComputeOption; }

private:
    /// The ProcessControl for this call.
    ProcessControl* mProcControl;

    /// The global time of the request.
    TimeFrame mGlobalTime;

    /// The desired width and height of the compute result, if applicable.
    u32 mWidth, mHeight;

    /// A generic compute option value for this call.
    u32 mComputeOption;

    /// The actual OpenAL context in the system.
    OpenALContext* mOpenALContext;
};

} // namespace Util
} // namespace WS
