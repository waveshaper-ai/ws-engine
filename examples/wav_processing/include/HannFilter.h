#include "AudioModel.h"
#include "BasicTypes.h"
#include <memory>

namespace WS
{
class HannFilter
{
public:
    HannFilter(u32 const filterWindowSize);

    /// @brief Receives a buffer of "filterWindowSize" containing the samples to be filtered.
    /// @brief Will return then Hann filtered data, in outSamples, processed with the given model.
    /// @param dataSamples a float buffer "filterWindowSize" "filterWindowSize" samples
    /// @param sampleCnt a value that specifies WIN_LEN
    /// @param model the actual model to use for processing
    /// @param outSamples an output buffer allocated for "filterWindowSize" samples.
    /// @return success / failed
    bool applyFilter(float* dataSamples, u32 sampleCnt, AudioModel& model, float* outSamples);

private:
    bool applyFilterInternal(float* dataSamples, u32 sampleCnt, AudioModel& model, float* outSamples);

    // Data members
private:
    u32 const mWindowSize;
    u32 const mHopSize;

    /// @brief  Holds the output values, keeping the last values for next hop
    std::unique_ptr<float> mOverlapBuffer;

    /// Declare and zero-fill the model input buffer
    std::unique_ptr<float> mModelInputBuffer;
};

} // namespace WS
