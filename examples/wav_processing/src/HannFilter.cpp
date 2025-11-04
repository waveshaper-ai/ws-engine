#include "HannFilter.h"
#include "Constants.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace
{
using HannFilter = WS::HannFilter;
}

HannFilter::HannFilter(u32 const filterWindowSize) : mWindowSize{filterWindowSize},
                                                     mHopSize{filterWindowSize / 2}, mOverlapBuffer{new float[filterWindowSize]}, mModelInputBuffer{new float[filterWindowSize]}
{
    std::memset(mOverlapBuffer.get(), 0, mWindowSize);
    std::memset(mModelInputBuffer.get(), 0, mWindowSize);
    
}

bool HannFilter::applyLastFilter(float* outSamples)
{
    std::memcpy(outSamples, mOverlapBuffer.get(), mWindowSize * sizeof(float));
    return true;
}

bool HannFilter::applyFilter(float* dataSamples, u32 sampleCnt, AudioModel& model, float* outSamples)
{
    if(sampleCnt != mWindowSize)
    {
        return false;
    }
    // Process first half of data [0 - mHopSize] in dataSamples
    if(applyFilterInternal(dataSamples, mHopSize, model, outSamples))
    {
        // Process second half of data [mHopSize - mWindowSize] in dataSamples
        return applyFilterInternal(dataSamples + mHopSize, mHopSize, model, outSamples + mHopSize);
    }

    return false;
}

bool HannFilter::applyFilterInternal(float* dataSamples, u32 sampleCnt, AudioModel& model, float* outSamples)
{
    if(sampleCnt != mHopSize)
    {
        return false;
    }

    std::memcpy(mModelInputBuffer.get(), mModelInputBuffer.get() + mHopSize, mHopSize * sizeof(float));
    std::memcpy(mModelInputBuffer.get() + mHopSize, dataSamples, mHopSize * sizeof(float));

    std::unique_ptr<float> windowOut{new float[mWindowSize]{0.0}};

    model.process(mModelInputBuffer.get(), windowOut.get());


    for(u32 s = 0; s < mWindowSize; s++)
    {
        float multiplier = 0.5f * (1 - std::cos(2 * WS::Util::Constants::Pi<float>{}() * s / (mWindowSize - 1)));
        windowOut.get()[s] *= multiplier;
    }

    // Add to the first half of the overlap buffer
    for(u32 s = 0; s < mHopSize; s++)
    {
        mOverlapBuffer.get()[s] += windowOut.get()[s];
        outSamples[s] = mOverlapBuffer.get()[s];
    }

    // Copy to overlap buffer for next hop
    std::memcpy(mOverlapBuffer.get(), static_cast<void*>(&windowOut.get()[mHopSize]), mHopSize * sizeof(float));
    return true;
}
