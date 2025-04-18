#ifndef _TL_RAPIDWRAPPER_H
#define _TL_RAPIDWRAPPER_H

#include "BasicTypes.h"
#include "LibScriptExportOs.h"
#include "ScriptException.h"
#include <string>
#include <vector>


namespace TL
{
namespace LibScript
{

class LIBSCRIPT_EXPORT RapidWrapper
{
    struct Internal;
    std::unique_ptr<Internal> mInternal;

    int mObjIndex{-1}; // used only when isArray() == true
    std::string mLastReadObjTag{""};

public:
    RapidWrapper();
    RapidWrapper(std::string const&);
    RapidWrapper(Internal& internal, std::string const& tag);
    RapidWrapper(Internal& internal, std::string const& tag, int index);
    RapidWrapper(Internal& internal, int index);

    ~RapidWrapper();

    using ObjectPair = std::pair<std::string, int>;
    using ObjectPairList = std::vector<ObjectPair>;
    using RapidWrapperPair = std::pair<RapidWrapper*, std::string>;

    /// Check if this wrapper is a document
    bool isDocument() const;

    /// Check if this wrapper is a value
    bool isValue() const;

    /// Check if this wrapper is an object
    bool isObject() const;

    /// Check if this wrapper is an array
    bool isArray() const;

    /// Initialize the document if it not yea an object
    /// Throws ScriptException::ExcType::JsonNotADocument if *this is not a document.
    void init();

    /// Reads the value in *this and puts that into "data" if
    /// type T match the rapidjson::Type of *this.
    /// Otherwise, throw a ScriptException::ExcType::JsonInvalidType
    /// Supported types: int, float, u32, bool and string
    template <class T>
    bool readValue(T& data);

    /// Returns RapidWrapper ptr with the specified rapidjson::Value.
    /// If not empty tag, return the next Value with the tag as given.
    /// If empty tag and *this is an array, return the next Value.
    /// Throw exception in other case or can not find object.
    RapidWrapper* readObject(std::string const& tag = "");

    /// Returns a wrapper of the array with given tag if exist.
    /// Throws ScriptException::ExcType::JsonInvalidTag if can not find the object
    /// Throws ScriptException::ExcType::JsonNotAnArray if the object is not an array
    RapidWrapper* readArray(std::string const& tag);

    /// Writes the data to *this
    /// Supported types: int, float, u32, bool and string
    template <class T>
    bool writeValue(T const& data);

    /// Instantiate an empty object with the given tag and return the wrapper with the object.
    RapidWrapper* writeObject(std::string const& tag);

    /// Instantiate an empty object with no tag and return the wrapper with the object.
    /// Can only be used for array.
    /// Throw ScriptException::ExcType::JsonNotAnArray if *this is not an array.
    RapidWrapper* writeObject();

    /// Instantiate an empty array with the given tag and return the wrapper with the array.
    RapidWrapper* writeArray(std::string const& tag);

    ///
    void objectToArray();

    /// Returns the size of the array if it is an array
    /// Throws ScriptException::ExcType::JsonNotAnArray otherwise
    int arraySize();

    /// Stream out the document to file or string
    void writeToFile(std::string const&);
    void writeToString(std::string& outJsonStr);
};

#ifdef OS_WINDOWS
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<bool>(bool const& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<s32>(s32 const& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<u32>(u32 const& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<float>(float const& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<double>(double const& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<s64>(s64 const& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<u64>(u64 const& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<std::string>(std::string const& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::writeValue<TL::LibScript::DataBuffer>(TL::LibScript::DataBuffer const& data);

template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<bool>(bool& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<s32>(s32& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<u32>(u32& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<float>(float& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<double>(double& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<s64>(s64& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<u64>(u64& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<std::string>(std::string& data);
template <>
LIBSCRIPT_EXPORT bool RapidWrapper::readValue<TL::LibScript::DataBuffer>(TL::LibScript::DataBuffer& data);
#endif

} // namespace LibScript
} // namespace TL

#endif