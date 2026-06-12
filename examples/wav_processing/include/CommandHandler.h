#pragma once

#include "JsonReader.h"
#include "JsonWriter.h"
#include "LibUtilExportOs.h"
#include "ScriptObject.h"
#include "Subject.h"
#include <stack>
#include <tuple>
#include <vector>

namespace WS
{
namespace Util
{
class DescriptorRegistry;

/// Example of the LibPipeline::ProcessNode class....
class LIBUTIL_EXPORT CommandHandler
{
    using CommandTriplet = std::tuple<std::string, JsonReader::ScriptFunc, JsonWriter::ScriptFunc>;
    using CommandTriplets = std::vector<CommandTriplet>;

    /// Subject to notify the change of the script object
    WS::Util::Subject<ScriptObject*> mScriptObjChangedSub;

public:
    static const std::string CREATE_NEW_OBJECT;
    static const std::string LOAD_OBJECT;
    static const std::string DELETE_OBJECT;
    static const std::string SELECT_OBJECT;
    static const std::string PARSE_NAMED_GROUP;

    CommandHandler(DescriptorRegistry& registry);
    ~CommandHandler();

    /// Register/Unregister new commands to be used by this instance of CommandHandler.
    bool registerCommand(std::string const& cmdName, JsonReader::ScriptFunc const& read, JsonWriter::ScriptFunc const& write);
    bool unregisterCommand(std::string const& cmdName);

    /// Single read/write command utility method.
    bool readCommand(std::string const& cmdName, WS::Util::JsonReader& reader);
    bool writeCommand(std::string const& cmdName,
        WS::Util::JsonWriter& writer, ScriptObject& obj);

    /// Single read/write command utility method for a given NamedGroup and its visitor.
    bool readCommandForNamedGroup(std::string const& cmdName,
        WS::Util::JsonReader& reader, NamedGroup& group, NamedGroup::VisitorFunctor visitorFunc);
    bool writeCommandForNamedGroup(std::string const& cmdName,
        WS::Util::JsonWriter& writer, NamedGroup& group, NamedGroup::VisitorFunctor visitorFunc);

    /// Loops through all known commands and read-them in sequentially, from a generic
    /// array-of-object with commands.
    bool readCommands(std::string const& arrayCmdName, WS::Util::JsonReader& reader);

    /// Loops through all known objects of the DescriptorRegisty and write-them out sequentially,
    /// within a generic array-of-object with commands.
    bool writeCommands(std::string const& arrayCmdName, std::string const& command,
        std::vector<ScriptObject*> objectsToWrite, WS::Util::JsonWriter& writer);

    /// Accessor
    inline DescriptorRegistry& getRegistry() { return mRegistry; }
    inline WS::Util::Subject<ScriptObject*>& GetScriptObjChangedSub() { return mScriptObjChangedSub; }

private:
    bool createNewObjectReaderCmd(JsonReader& reader);
    bool createNewObjectWriterCmd(JsonWriter& writer);
    bool loadObjectReaderCmd(JsonReader& reader);
    bool loadObjectWriterCmd(JsonWriter& writer);
    bool deleteObjectReaderCmd(JsonReader& reader);
    bool deleteObjectWriterCmd(JsonWriter& writer);
    bool selectObjectReaderCmd(JsonReader& reader);
    bool selectObjectWriterCmd(JsonWriter& writer);
    bool parseNamedGroupReaderCmd(JsonReader& reader);
    bool parseNamedGroupWriterCmd(JsonWriter& writer);

    /// Usefull method to find a command in the vector and return it.
    /// Cmd name is empty if tripplet command not found.
    CommandTriplets::iterator findCommand(std::string const& cmdName);

private:
    CommandTriplets mCommands;
    DescriptorRegistry& mRegistry;

    /// The current object being selected by SelectObject.
    /// When in saving procedure, mCurrentObj takes the values
    /// of whatever object being saved, and is set to nullptr after.
    ScriptObject* mCurrentObj;

    /// The current group being worked on, selected by
    /// SelectObject command (or read/writeCommandForNamedGroup)
    NamedGroup* mCurrentGroup;
    WS::Util::NamedGroup::VisitorFunctor mGroupVisitor;

    /// The actual VisitorHelper to be used to visit mCurrentGroup,
    /// say some NamedValue<types> have custom types from users of lib-script.
    std::unique_ptr<WS::Util::NamedGroup::VisitorHelper> mVisitorHelper;
};

class JsonReaderVisitor
{
    using GroupPair = std::tuple<WS::Util::NamedGroup*, int>;

public:
    JsonReaderVisitor(WS::Util::JsonReader& reader) : mReader{reader} {}
    ~JsonReaderVisitor() = default;

    template <class T>
    void onNamedObject(T& obj)
    {
        GroupPair& pair{mGroups.top()};
        std::get<1>(pair)--;
        bool popGroup{std::get<1>(pair) == 0};
        mReader.readNamedValue(obj);
        if(popGroup)
        {
            if(!std::get<0>(pair)->containsSameNames())
            {
                mReader.readObjectEnd();
            }
            else
            {
                mReader.readArrayEnd();
            }
            mGroups.pop();
        }
    }

private:
    std::stack<GroupPair> mGroups;
    WS::Util::JsonReader& mReader;
};

class JsonWriterVisitor
{
    using GroupPair = std::tuple<WS::Util::NamedGroup*, int>;

public:
    JsonWriterVisitor(WS::Util::JsonWriter& writer) : mWriter{writer} {}
    ~JsonWriterVisitor()
    {
        while(!mGroups.empty())
        {
            if(!std::get<0>(mGroups.top())->containsSameNames())
            {
                mWriter.writeObjectEnd();
            }
            else
            {
                mWriter.writeArrayEnd();
            }
            mGroups.pop();
        }
    }
    template <class T>
    void onNamedObject(T& obj)
    {
        GroupPair& pair{mGroups.top()};
        std::get<1>(pair)--;
        bool popGroup{std::get<1>(pair) == 0};
        mWriter.writeNamedValue(obj);
        if(popGroup)
        {
            if(!std::get<0>(pair)->containsSameNames())
            {
                mWriter.writeObjectEnd();
            }
            else
            {
                mWriter.writeArrayEnd();
            }
            mGroups.pop();
        }
    }

private:
    WS::Util::JsonWriter& mWriter;
    std::stack<GroupPair> mGroups;
};

template <>
void LIBUTIL_EXPORT WS::Util::JsonReaderVisitor::onNamedObject(WS::Util::NamedGroup& obj);
template <>
void LIBUTIL_EXPORT WS::Util::JsonWriterVisitor::onNamedObject(WS::Util::NamedGroup& obj);

} // namespace Util
} // namespace WS
