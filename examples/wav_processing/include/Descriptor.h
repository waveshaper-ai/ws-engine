#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <string>
#include <functional>

namespace WS
{
namespace Util
{
class ScriptObject;

/// The Descriptor is used to instantiate Scriptable objects.
/// It holds the general scriptable class for which
/// each Scriptable objects are from.
class LIBUTIL_EXPORT Descriptor
{
public:
    using FactoryFctor = std::function<ScriptObject*(u32)>;
    Descriptor(std::string const& name);
    Descriptor(std::string const& name, FactoryFctor const& ftor);
    Descriptor(Descriptor const&) = delete;
    Descriptor(Descriptor&&) = delete;
    Descriptor& operator=(Descriptor const&) & = delete;
    Descriptor& operator=(Descriptor&&) & = delete;
    virtual ~Descriptor() = default;

    /// The main overridable factory method if derivation is used.
    /// Base class checks for a valid FactoryFctor and uses it if good.
    virtual ScriptObject* createScriptable(u32 uniqueId);

    /// Clean up the script object if necessery, before deleting
    /// the instance. Base class does nothing.
    virtual void cleanupInstance(WS::Util::ScriptObject* obj);

    inline std::string getClassName() const { return mClassName; }

private:
    std::string mClassName;
    FactoryFctor mFactoryFctor;
};
} // namespace Util
} // namespace WS
