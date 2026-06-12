#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <string>

namespace WS
{
namespace Util
{
/// AudioHWBuffer class for holding OpenAL audio buffers in the system.
/// The user generates the audio chunk data to then store on the sound device
/// the data (OpenAL makes a copy) and returns a mBufferId.
/// Class holds and defines how audio buffers are handled. Uses OpenAL
/// implementation to manage soundcard resources.
/// Methods allow to play and stop the sound buffer.
class LIBUTIL_EXPORT AudioHWBuffer
{
public:
    enum class Channels
    {
        MONO = 1,
        STEREO,
        MONO_LEFT,
        MONO_RIGHT
    };

    AudioHWBuffer();
    virtual ~AudioHWBuffer();

    /// Operations
    virtual bool setupBuffer(u8* chunkData, u32 chunkSize,
        Channels ch, u32 depth, u32 sampleRateHz, bool setupSingleSrc);
    virtual bool hardwareReady();
    virtual bool playBuffer(u32 channel, float volume);
    virtual void stopBuffer();
    virtual bool isPlaying();
    virtual bool queueBuffer(u32 sourceId);
    virtual bool unQueueBuffer(u32 sourceId);

    /// Accessors
    u32 getBufferId() const { return mBufferId; }
    u32 getSourceId() const { return mSourceId; }

    // Data members
private:
    /// OpenAL buffer name (unique id) and its associated source.
    u32 mBufferId;
    u32 mSourceId;
};

} // namespace Util
} // namespace WS
