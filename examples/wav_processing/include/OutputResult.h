#pragma once

#include "AudioPacket.h"
#include "AudioSpec.h"
#include "BasicTypes.h"
#include <array>
#include <atomic>
#include <tuple>
#include <vector>

namespace WS
{
namespace Util
{
/// This OutputResult will be able to contain an AudioPacket, which is in
/// fact a CachedEntry<AudioSpec,AudioSpec::Storage>.
/// ParamContext holds input parameters to the processing as well
/// as different context necessary (e.g. GPU OpenCL, OpenGL, etc)
class LIBUTIL_EXPORT OutputResult
{
public:
    static const u32 ARRAY_SIZE{10};

    OutputResult();
    virtual ~OutputResult() = default;

    template <class TYPE>
    bool addPacket(u32 index, TYPE* const& packet)
    {
        if(index < ARRAY_SIZE)
        {
            addPacketInContainer<TYPE>(index, packet);
            return true;
        }
        return false;
    }

    template <class TYPE>
    bool getPacket(u32 index, TYPE*& packet)
    {
        if(index < ARRAY_SIZE)
        {
            packet = getPacketFromContainer<TYPE>(index);
            return true;
        }
        return false;
    }

protected:
    template <class TYPE>
    void addPacketInContainer(u32 index, TYPE* const& packet);

    template <class TYPE>
    TYPE* getPacketFromContainer(u32 index);

    /// Container of possibly many AudioPacket. AudioPacket is in fact
    /// a simple pointer to a CachedEntry<AudioSpec, AudioSpec::Storage>.
    std::array<AudioPacket*, ARRAY_SIZE> mAudioPackets;
};

/// Usefull method to append rhs packet at index 0 from given OUTPUTRESULT
/// to a target OUTPUTRESULT
template <class OUTPUTRESULT>
bool appendPacket0AtIndex(OUTPUTRESULT& target, u32 index, OUTPUTRESULT& rhs);

#ifdef OS_WINDOWS

template <>
LIBUTIL_EXPORT bool WS::Util::appendPacket0AtIndex<OutputResult>(OutputResult& target, u32 index, OutputResult& rhs);
template <>
LIBUTIL_EXPORT void OutputResult::addPacketInContainer<AudioPacket>(u32, AudioPacket* const& packet);
template <>
LIBUTIL_EXPORT AudioPacket* OutputResult::getPacketFromContainer<AudioPacket>(u32);

#endif
} // namespace Util
} // namespace WS
