#pragma once

#include "DynArray.h"
#include "AudioSpec.h"

namespace WS
{
namespace Sdk
{
class AudioCache
{
    static const u32 MAX_SIZE{3U};

public:
    AudioCache(Util::AudioSpec const& spec) : mPackets{spec}, mLastFrame{0U}, mSpec{spec}
    {
    }

    Util::AudioPacket* generatePacket()
    {
        if(mLastFrame == MAX_SIZE)
        {
            mLastFrame = 0U;
        }
        Util::AudioPacket* ap{&mPackets[mLastFrame++]};
        if(!ap->isValid())
        {
            ap->operator=(Util::AudioPacket{mSpec});
        }
        return ap;
    }

    Util::AudioPacket* getPacket(u32 frame)
    {
        if(frame < MAX_SIZE)
        {
            return &mPackets[frame];
        }
        // Invalid frame
        return nullptr;
    }

private:
    WS::Util::DynArray<Util::AudioPacket, MAX_SIZE> mPackets;
    u32 mLastFrame;
    Util::AudioSpec mSpec;
};

} // namespace Util
} // namespace WS
