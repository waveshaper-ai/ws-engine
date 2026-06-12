#pragma once

#include "AudioHWBuffer.h"
#include "AudioPacket.h"
#include "TimeFrame.h"
#include <memory>
#include <string>

namespace WS
{
namespace Util
{
/// AudioSpec implements a simple interface used to report information about
/// an AudioHWBuffer containing audio chunk data formatted in a certain way.
/// The interface reports the number of bytes used for a “frame” of audio.
class LIBUTIL_EXPORT AudioSpec
{
public:
    using Channels = AudioHWBuffer::Channels;
    AudioSpec();
    AudioSpec(Channels ch, u32 depthInBits, u32 samples, u32 sampleRate, u32 totalSampleDuration);
    AudioSpec(AudioSpec const& rhs) = default;
    virtual ~AudioSpec() = default;

    /// Simple accessors
    inline u32 getSampleRate() const { return mSampleRate; }
    inline u32 getNbSamples() const { return mFrameSampleCnt; }
    inline u32 getDepth() const { return mDepth; }
    inline AudioHWBuffer::Channels getChannel() const { return mChannel; }
    inline u32 getNbSamplesForSize(u32 size) const
    {
        return size / (((mChannel == AudioHWBuffer::Channels::MONO) ? 1 : 2) * (mDepth / 8));
    }
    inline bool isStereo() const
    {
        return mChannel == AudioHWBuffer::Channels::STEREO;
    }
    inline u64 getTotalSamples() const { return mTotalSamples; }
    inline Util::TimeFrame getTotalDuration() const { return Util::TimeFrame(mTotalSamples, mSampleRate * 100); }

    /// overrides
    virtual u32 getSizeForFrame() const;
    virtual std::string getSpecName() const;

    /// Operation on spec
    bool isSame(AudioSpec const& rhs) const;

    static AudioPacket* allocateDataSet(AudioSpec const& spec)
    {
        return new AudioPacket{spec.getNbSamples(), spec.getChannel() == AudioHWBuffer::Channels::STEREO};
    }

    /// Us e notification for the reInitEntry.
    /// onEntryReInited(CacheID<AudioSpec>, Storage*) ?

    // Data members
private:
    /// Mono or stereo
    AudioHWBuffer::Channels mChannel;

    /// Channel bit depth. May be 8, 10, 12 or 16.
    u32 mDepth;

    /// The number of samples for a frame of samples, for one channel
    u32 mFrameSampleCnt;

    /// The sample rate in HZ.
    u32 mSampleRate;

    /// The total number of samples for the entire AudioSpec stream.
    u32 mTotalSamples;
};

} // namespace Util
} // namespace WS