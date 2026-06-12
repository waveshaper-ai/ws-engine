#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <condition_variable>
#include <mutex>
#include <string>

namespace WS
{
namespace Util
{
class LIBUTIL_EXPORT SyncEvent
{
    using AccessLock = std::unique_lock<std::mutex>;
    using AccessGuard = std::lock_guard<std::mutex>;

public:
    inline SyncEvent(std::string const& name) : mEvent{false}, mWaitSignal{}, mCondVar{}, mName(name)
    {
    }
    inline ~SyncEvent() = default;

    inline bool waitForEvent(u32 maxTimeToWaitMS)
    {
        using Clock = std::chrono::steady_clock;
        const Clock::duration msDuration{std::chrono::milliseconds{maxTimeToWaitMS}};

        AccessLock lock(mWaitSignal);

        if(maxTimeToWaitMS == 0)
        {
            // true: Event was set, false, timedout.
            mCondVar.wait(lock, [this]
                { return mEvent; });
            return true;
        }
        else
        {
            // true: Event was set, false, timedout.
            auto now{Clock::now()};
            return mCondVar.wait_until(lock, now + msDuration, [this]
                { return mEvent; });
        }
    }

    inline void setEvent()
    {
        mEvent = true;
        mCondVar.notify_all();
    }

    inline void resetEvent()
    {
        mEvent = false;
        mCondVar.notify_all();
    }

private:
    bool mEvent;
    std::mutex mWaitSignal;
    std::condition_variable mCondVar;
    std::string mName;
};
} // namespace Util
} // namespace WS
