#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"

namespace WS
{
namespace Util
{
/// LibCore::TimeFrame encapsulates 3 simple u32s to describe the time using a
/// FrameRate describe as rate numerator on rate denominator, and a frame number.
class LIBUTIL_EXPORT TimeFrame
{
public:
    class LIBUTIL_EXPORT FrameRate
    {
    public:
        FrameRate();
        FrameRate(double rate);
        FrameRate(u32 frameRateNum);
        FrameRate(const FrameRate& rhs);

        /// Assignment and comparison operators
        FrameRate& operator=(const TimeFrame::FrameRate& rhs);
        bool operator==(const TimeFrame::FrameRate& rhs) const;
        bool operator!=(const TimeFrame::FrameRate& rhs) const;

        /// Accessors in many flavors
        double getFrameRate() const;
        inline u32 getRateNum() const { return _rateNum; };
        void setFrameRate(double frameRateFPS);
        inline void setRateNum(u32 frameRateNum) { _rateNum = frameRateNum; };

        // Data members
    private:
        u32 _rateNum; // Numerator in 100 cycles / sec.
        static const u32 RATE_DEN; // Common denominator. Currently 100.
    };

public:
    TimeFrame();
    TimeFrame(u32 frame, double frameRate);
    TimeFrame(u32 frame, u32 frameRateNum = 3000UL);
    TimeFrame(const TimeFrame& rhs);
    ~TimeFrame();

    /// Assignment operators.
    TimeFrame& operator=(const TimeFrame& rhs);

    /// Comparison operators.
    bool operator==(const TimeFrame& rhs) const;
    bool operator!=(const TimeFrame& rhs) const;
    bool operator>(const TimeFrame& rhs) const;
    bool operator<(const TimeFrame& rhs) const;
    bool operator>=(const TimeFrame& rhs) const;
    bool operator<=(const TimeFrame& rhs) const;

    /// Increment the time by another time amount.
    TimeFrame operator+(const TimeFrame& rhs) const;
    TimeFrame operator-(const TimeFrame& rhs) const;
    TimeFrame& operator+=(const TimeFrame& rhs);
    TimeFrame& operator-=(const TimeFrame& rhs);
    TimeFrame operator*(float mulFactor) const;

    /// Increment the frame number.
    const TimeFrame& operator+(int frameIncrement) const;
    const TimeFrame& operator-(int frameIncrement) const;
    TimeFrame& operator++();
    TimeFrame& operator--();
    TimeFrame& operator+=(int frameIncrement);
    TimeFrame& operator-=(int frameIncrement);

    /// Returns the inifinite value for this TimeFrame.
    /// It is -1, framerate.
    inline TimeFrame TO_INFINITE() const { return TimeFrame(u32(-1), _rate.getRateNum()); }
    inline bool IS_INFINITE() const { return operator==(TimeFrame(u32(-1), _rate.getRateNum())); }

    /// Accessors
    double getTime() const;
    inline u32 getFrame() const { return _frame; }
    double getFrameRate() const { return _rate.getFrameRate(); }
    inline u32 getRateNum() const { return _rate.getRateNum(); }
    inline void getTimecode(u32& hours, u32& mins, u32& secs, u32& frames) const
    {
        u32 frame = getFrame();
        u32 frameRate = static_cast<u32>(getFrameRate());

        // Adjust NTSC drop-frame timecode
        if(_rate.getRateNum() == 2997)
        {
            // Add 2 frames for every minute, except every tenth.
            u32 min = 0;
            u32 frCnt = frame;
            while(frCnt >= 1800)
            {
                min++;
                if(min < 10)
                {
                    frame += 2;
                    frCnt += 2;
                }
                else
                {
                    min = 0;
                }
                frCnt -= 1800;
            }
            frameRate = 30;
        }

        if(frameRate != 0)
        {
            frames = static_cast<u32>(frame % frameRate);
            secs = static_cast<u32>(frame / frameRate) % 60;
            mins = static_cast<u32>(frame / (frameRate * 60)) % 60;
            hours = static_cast<u32>(frame / (frameRate * 3600)) % 60;
        }
        else
        {
            frames = 0;
            secs = 0;
            mins = 0;
            hours = 0;
        }
    }

    /// Set methods many flavors.
    void setFrameRate(double frameRate);
    inline void setFrame(u32 frame) { _frame = frame; }
    void setTime(double time); // Another way to set the frame

    // Data Members
private:
    u32 _frame;
    FrameRate _rate;
};

} // namespace Util
} // namespace WS