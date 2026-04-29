#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <memory>
#include <utility>

namespace WS
{
namespace Util
{
class AudioSpec;

/// AudioPacket is used to store left and right data buffers with "sampleCnt" entries.
class LIBUTIL_EXPORT AudioPacket
{
public:
    AudioPacket();
    AudioPacket(u32 sampleCnt, bool isStereo);
    AudioPacket(AudioSpec const& spec);

    /// Construct a packet with the data provided (L, or L and R) -- does not take ownership of the data.
    /// Make sure the buffer(s) referenced is/are alive throughout usage of AudioPacket.
    /// Set dataPrefilled to update the accumulated samples to "sampleCnt" as data is already in buffer.
    AudioPacket(u32 sampleCnt, float* dataToUseL, float* dataToUseR = nullptr, bool dataPrefilled = false);
    AudioPacket(AudioPacket const& rhs);

    /// Appends the given "data" inside this AudioPacket buffer.
    /// Returns the total amount of samples accumulated
    u32 accumData(AudioPacket const& data, u32 startOffset = 0U);

    /// When filling the buffer from an external source (and skipping the fact to copy that data)
    /// use assingAccumData() to tell the AudioPacket you've accumulated samples in the buffer.
    u32 assignAccumData(u32 amountOfAccumSamplesToAssign);

    /// Removes the amount of samples specified off the accumulation buffer, and reset
    /// the new mAccumSample, which is returned. The remaining data in the buffer
    /// is moved, and the rest of the buffer is reset to 0,
    u32 removeConsumedData(u32 samplesConsumed);

    /// Resets the accumulated samples counter, and does not touch the internal buffer.
    /// When mapping existing buffers, sometimes you want them ledt untouched.
    void resetAccumSamples() { mAccumSamples = 0U; }

    // Ability to copy operator
    AudioPacket& operator=(AudioPacket const& rhs);

    // Accessors
    inline u32 sampleCnt() const { return mSampleCnt; }
    inline u32 accumSamples() const { return mAccumSamples; }
    inline bool isStereo() const { return mBufferToUseR != nullptr; }
    inline bool isValid() const { return sampleCnt() > 0; }
    inline float* bufL(u32 offsetInSamples = 0U) const { return mBufferToUseL + offsetInSamples; }
    inline float* bufR(u32 offsetInSamples = 0U) const { return mBufferToUseR + offsetInSamples; }

private:
    std::unique_ptr<float[]> mBufferL;
    std::unique_ptr<float[]> mBufferR;
    float* mBufferToUseL{nullptr};
    float* mBufferToUseR{nullptr};
    u32 mAccumSamples{0U};
    u32 mSampleCnt{0U};
};

} // namespace Util
} // namespace WS
