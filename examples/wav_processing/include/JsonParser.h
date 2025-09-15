#ifndef _WS_JSONPARSER_H
#define _WS_JSONPARSER_H

#include "LibUtilExportOs.h"
#include <string>

namespace WS
{
namespace Util
{
class Scriptable;

class LIBUTIL_EXPORT JsonParser
{
public:
    JsonParser() = default;
    ~JsonParser() = default;

    /// Generates a script file of name "pathToFile" using the given Scriptable.
    /// A JsonWriter object is used to orchestrate the writing data in the script.
    static bool generateScript(std::string const& pathToFile, Scriptable&);
    static bool generateScriptStr(std::string& outScriptStr, Scriptable&);

    /// Loads-in the given "pathToFile" and the reads-in the given "subValueTag" into
    /// the "Scriptable". Throws ScriptException on issues.
    /// A JsonReader object is used to orchestrate the reading of the data in the Scriptable objects.
    static bool parseScript(std::string const& pathToFile, std::string const& subValueTag, Scriptable&);
    static bool parseScriptStr(std::string const& jsonContent, std::string const& subValueTag, Scriptable&);

    /// Reads-in/write-out a Json file and transfer its content to/from given outJSonString/inJsonString.
    static bool readFileInStr(std::string const& pathToFile, std::string& outJsonString);
    static bool writeFileFromStr(std::string const& pathToFile, std::string const& inJsonString);
};

} // namespace Util
} // namespace WS

#endif
