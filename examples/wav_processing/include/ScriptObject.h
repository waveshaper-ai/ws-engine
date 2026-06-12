#pragma once

#include "BasicTypes.h"
#include "NamedGroup.h"
#include "Scriptable.h"

namespace WS
{
namespace Util
{
class LIBUTIL_EXPORT ScriptObject : public Scriptable
{
public:
    ScriptObject(const u32 uniqueId);
    virtual ~ScriptObject() = default;

    /// Accessors
    inline u32 getId() const { return mUniqueId; }
    inline NamedGroup& getRootGroup() { return mRootGroup; }
    inline NamedGroup const& getRootGroup() const { return mRootGroup; }

private:
    /// A uniqueId within the context of a system
    u32 mUniqueId;

    /// The root group, which will contain other groups.
    NamedGroup mRootGroup;
};

} // namespace Util
} // namespace WS
