#pragma once

#include "ComponentException.h"
#include "LibUtilExportOs.h"

namespace WS
{
namespace Util
{
/// Holds all resources necessary for an OpenALContext, which embodies the audio
/// functions from OpenAL. OAL context should be created and initialized
/// before using any audio methods - otherwise it won't work.
class LIBUTIL_EXPORT OpenALContext
{
public:
    OpenALContext();
    virtual ~OpenALContext();

    /// Initializes a brand new OpenAL context. This context is valid until
    /// "Destroy" is called or the instance is deleted.
    bool initialize();
    void destroy();

    // Accessors

    // Data members
private:
    struct OS;
    OS& _os;
};

} // namespace Util
} // namespace WS
