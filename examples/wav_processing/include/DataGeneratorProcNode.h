#pragma once

#include "AIProcessNode.h"
#include "AudioCache.h"
#include "Constants.h"
#include <cmath>

#define M_PI Util::Constants::Pi<float>{}()

namespace WS
{
namespace Sdk
{
class DataGeneratorProcNode : public Util::ProcessNode
{
public:
    DataGeneratorProcNode(u32 id, u32 waveFrequencyHz = 10'000)
        : ProcessNode{id, "DataGeneratorPN"}, mSpec{Util::AudioSpec::Channels::STEREO, 32U, 1024U, 48'000U, 480'000U}, mAudioCache{mSpec}, mWaveFormFreqHz{waveFrequencyHz}
    {
    }

    bool computeResult(Util::ParamContext& ctx, Util::OutputResult& outResult) override
    {
        /// My container!
        setProgressCompletion(0);
        Util::TimeFrame localTime{ctx.getGlobalTime()};
        localTime.setFrame(localTime.getFrame() % 3);

        // Produce an entry in the cache and output it.
        Util::AudioPacket* packet{nullptr};
        // Fill-in the packet
        packet = mAudioCache.generatePacket();
        generateFrameData(packet->bufL(), packet->sampleCnt());
        // Now entry exists! return it (either from cache or brand new!)
        outResult.addPacket(localTime.getFrame(), packet);
        setProgressCompletion(100);
        return true;
    }

    /// Must be overiden: will be called to stream object from json reader.
    bool readFromStream(Util::JsonReader&) override { return true; }
    bool writeToStream(Util::JsonWriter&) override { return true; }

    void cleanupCachedPacket() override
    {
    }

    static void generateFrameData(float* data, u32 sampleCnt, bool sinFunction = true, size_t startTime = 0U)
    {
        if(sinFunction)
        {
            float freq_1 = 340.F;
            float freq_2 = 134.F;
            float w_1 = 2 * M_PI * freq_1;
            float w_2 = 2 * M_PI * freq_2;
            for(size_t i = startTime; i < (startTime + sampleCnt); ++i)
            {
                float time{float(i) / 48000.F};
                data[i - startTime] = 0.5 * sin(w_1 * time) + 0.3 * sin(w_2 * time);
            }
        }
        else
        {
            for(size_t i = startTime; i < (startTime + sampleCnt); ++i)
                data[i - startTime] = 0.09F;
        }
    }

private:
    Util::AudioSpec mSpec;
    AudioCache mAudioCache;
    u32 mWaveFormFreqHz;
};
} // namespace Sdk
} // namespace WS
