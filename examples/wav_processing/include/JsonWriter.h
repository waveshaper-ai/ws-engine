#ifndef _WS_JSONWRITER_H
#define _WS_JSONWRITER_H

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include "RapidWrapper.h"

#include <functional>
#include <string>

namespace WS
{
namespace Util
{
class Scriptable;

class LIBUTIL_EXPORT JsonWriter
{
protected:
    std::vector<RapidWrapper::RapidWrapperPair> mWrapperStack;

public:
    JsonWriter(RapidWrapper* wrapper)
    {
        mWrapperStack.push_back({wrapper, "TopOfStack"});
    }
    ~JsonWriter();

    using ScriptFunc = std::function<bool(JsonWriter&)>;

    /// Writes value pair to the wrapper at the back of the stack
    template <class T>
    bool writeValuePair(std::string const& tag, T const& value)
    {
        RapidWrapper* wrapper{mWrapperStack.back().first->writeObject(tag)};
        if(!wrapper)
        {
            return false;
        }
        return wrapper->writeValue(value);
    }

    /// Push the value to the wrapper at the back of the stack
    /// Throw ScriptException::ExcType::JsonNotAnArray if the ending stack is not an array
    template <class T>
    bool writeValue(T const& value)
    {
        RapidWrapper* wrapper{mWrapperStack.back().first->writeObject()};
        if(!wrapper)
        {
            return false;
        }
        return wrapper->writeValue(value);
    }

    template <class T>
    bool writeNamedValue(T& namedValue);

    /// Instantiate an empty array with the given tag and stack the array wrapper at the back of stack.
    bool writeArrayBegin(std::string const& tag);

    /// End the editing of the array.
    /// Pop all stacks till the array with the given tag is poped
    void writeArrayEnd();

    /// Writes-in an array of simple types.
    template <class T>
    bool writeArray(std::string const& tag, std::vector<T> const& vec)
    {
        writeArrayBegin(tag);
        for(auto const& it : vec)
        {
            writeValue(it);
        }
        writeArrayEnd();
        return !vec.empty();
    }

    /// Writes-in an array of unnamed objects simple types.
    template <class T>
    bool writeArrayOfObjects(std::string const& tag, std::vector<T> const& vec, std::function<void(JsonWriter&, T const&)> const& scriptToWriteObject)
    {
        writeArrayBegin(tag);
        for(auto const& it : vec)
        {
            if(writeObjectBegin(""))
            {
                // Call the method to continue parsing
                scriptToWriteObject(*this, it);
                writeObjectEnd();
            }
        }
        writeArrayEnd();
        return !vec.empty();
    }

    /// Starts an empty object with the given tag.
    /// A ScriptFunc is required to edit the content of the object
    bool writeObjectBegin(std::string const& name);
    void writeObjectEnd();
    bool writeObject(std::string const& name, Scriptable& script);
    bool writeObject(std::string const& tag, ScriptFunc const& script);

    /// Write the document to file
    void writeToFile(std::string const&);
    void writeToString(std::string& outJsonStr);

private:
    void popBack();
};

} // namespace Util
} // namespace WS

#endif