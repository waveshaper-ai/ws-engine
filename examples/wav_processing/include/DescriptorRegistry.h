#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace WS
{
namespace Util
{
class Descriptor;
class ScriptObject;

/// The DescriptorRegistry is used to instantiate Scriptable objects.
/// It Descriptor class holds the general scriptable class for which
/// each Scriptable objects are from.
class LIBUTIL_EXPORT DescriptorRegistry final
{
    using DescriptorPtr = std::unique_ptr<Descriptor>;
    using ScriptDescCont = std::vector<DescriptorPtr>;
    using ScriptObjectPtr = std::unique_ptr<ScriptObject>;
    using ScriptObjectPair = std::pair<ScriptObjectPtr, Descriptor*>;
    using ScriptObjectPairCont = std::vector<ScriptObjectPair>;

    DescriptorRegistry(DescriptorRegistry const&) = delete;
    DescriptorRegistry(DescriptorRegistry&&) = delete;
    DescriptorRegistry& operator=(DescriptorRegistry const&) & = delete;
    DescriptorRegistry& operator=(DescriptorRegistry&&) & = delete;

public:
    DescriptorRegistry();
    ~DescriptorRegistry();

    using ScriptObjectPtrPair = std::pair<ScriptObject*, Descriptor*>;
    using VisitorFunc = std::function<void(ScriptObjectPtrPair const& pair)>;

    bool addDescriptor(DescriptorPtr sd);

    inline bool addScriptObject(ScriptObjectPtr so, Descriptor* d)
    {
        mScriptObjectPairs.push_back({std::move(so), d});
        return true;
    }

    /// Returns a pointer to the descriptor for the given className, if exists.
    /// Return nullptr if none.
    Descriptor* getDescriptor(std::string const& className) const;

    /// Returns a pointer to the descriptor for the given className, if exists.
    /// Return nullptr if none.
    template <class DESCDERIVEDCLASS>
    DESCDERIVEDCLASS* getDescriptor(std::string const& className) const
    {
        return dynamic_cast<DESCDERIVEDCLASS*>(getDescriptor(className));
    }

    /// Returns a pointer to the scriptable object for the given id, if exists.
    /// Return nullptr if none.
    ScriptObject* getScriptObject(const u32 id) const;

    /// Returns a pointer to the descriptor for the given script object id, if exists.
    /// Return nullptr if none.
    Descriptor* getScriptObjDescriptor(const u32 id) const;
    /// Returns a pointer to the scriptable object for the given id, if exists.
    /// Return nullptr if none.
    ScriptObjectPtrPair getScriptObjectPair(const u32 id) const;

    /// To process each script object with the given func.
    /// For example, save each object to json
    void visitScriptObjects(VisitorFunc func) const;

    /// Returns the ScriptObject dynamically casted to its
    /// DERIVEDCLASS.
    template <class OBJDERIVEDCLASS>
    OBJDERIVEDCLASS* getScriptObject(const u32 id) const
    {
        return dynamic_cast<OBJDERIVEDCLASS*>(getScriptObject(id));
    }

    /// Remove objects and descriptors.
    /// Note that any references to these object/descriptor will become invalid.
    bool removeScriptObject(const u32 id);
    bool removeDescriptor(std::string const& className);

private:
    /// The list of Descriptor for each class that can be "Scriptable".
    /// Typically, the ScriptDescriptors are registered at the beginning
    /// of the program by a series of calls to "addDescriptor()".
    ScriptDescCont mDescriptors;

    /// This is the list of unique pointers to each Scriptable objects
    /// that have been instantiated in the system.
    ScriptObjectPairCont mScriptObjectPairs;
};

} // namespace Util
} // namespace WS
