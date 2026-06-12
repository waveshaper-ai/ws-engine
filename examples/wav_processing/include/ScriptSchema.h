#pragma once

#include "BasicTypes.h"
#include "CommandHandler.h"
#include "Scriptable.h"
#include <functional>
#include <string>

namespace WS
{
namespace Util
{
class DescriptorRegistry;

/// The ScriptSchema is used to drive the basic functionality
/// to load-in/generate scripts. It can be overriden to implement
/// more sections in a Json script. This base instance enforces the
/// following script schema:
/// A first value pair of WSScriptVersion/float for 1.0, 1.1, 2.0, etc.
/// then an object named "MiscObjects", which contains an array of the
/// following possible commands: CreateNewObject, LoadObject, DeleteObject.
/// The DescriptorRegistry keeps tracks of all created objects and
/// their associated Descriptors.
class LIBUTIL_EXPORT ScriptSchema : public WS::Util::Scriptable
{
public:
    /// Sets the version of the schema this instance is made for.
    ScriptSchema(WS::Util::DescriptorRegistry& registry, u32 majVersion = 1, u32 minVersion = 0);
    virtual ~ScriptSchema() = default;

    /// Must be overiden: will be called to stream object from json reader.
    bool readFromStream(WS::Util::JsonReader& reader) override;
    bool writeToStream(WS::Util::JsonWriter& writer) override;

    inline DescriptorRegistry& getRegistry() { return mCmdHandler.getRegistry(); }
    inline WS::Util::CommandHandler& getCmdHandler() { return mCmdHandler; }

protected:
    bool readMiscObjectsTreeCommands(WS::Util::JsonReader& reader);
    /// Loops through all known objects of the DescriptorRegisty and write-them out sequentially,
    /// within a generic array-of-object with commands. Give a filterIn method based on the
    /// uniqueId of the object to be saved (or not). All objcts written out if no "includeFilter" given.
    bool writeCommandForRegistryObjects(std::string const& arrayCmdName, std::string const& command,
        WS::Util::JsonWriter& writer, std::function<bool(u32 id)> includeFilter = nullptr);

    /// Useful method to write the version in the script.
    /// readScriptVersion returns true if the script version is a valide one for this schema.
    bool readScriptVersion(WS::Util::JsonReader& reader);
    void writeScriptVersion(WS::Util::JsonWriter& writer);

    // Data member
protected:
    WS::Util::CommandHandler mCmdHandler;

private:
    u32 mMajorVersion;
    u32 mMinorVersion;
};

} // namespace Util
} // namespace WS
