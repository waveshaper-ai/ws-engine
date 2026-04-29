#pragma once

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
/// This OutputSpec contains a list of packet specifications returned
/// by a ProcessNode.
class LIBUTIL_EXPORT OutputSpec
{
public:
    static const u32 ARRAY_SIZE{10};

    OutputSpec();
    virtual ~OutputSpec() = default;

    template <class SPEC>
    bool getSpec(u32 index, SPEC const*& spec)
    {
        if(index < ARRAY_SIZE)
        {
            spec = this->getSpecFromContainer<SPEC>(index);
            return true;
        }
        return false;
    }

    template <class SPEC>
    bool addSpec(u32 index, SPEC const* const& spec)
    {
        if(index < ARRAY_SIZE)
        {
            this->addSpecToContainer<SPEC>(index, spec);
            return true;
        }
        return false;
    }

protected:
    template <class SPEC>
    void addSpecToContainer(u32 index, SPEC const* const& spec);

    template <class SPEC>
    SPEC const* getSpecFromContainer(u32 index);

    /// Container of possibly many AudioPacket. AudioPacket is in fact
    /// a simple pointer to a CachedEntry<AudioSpec, AudioSpec::Storage>.
    std::array<AudioSpec const*, ARRAY_SIZE> mAudioSpecs;
};

#ifdef OS_WINDOWS

template <>
LIBUTIL_EXPORT void OutputSpec::addSpecToContainer<AudioSpec>(u32, AudioSpec const* const& packet);
template <>
LIBUTIL_EXPORT AudioSpec const* OutputSpec::getSpecFromContainer<AudioSpec>(u32);

#endif
} // namespace Util
} // namespace WS
