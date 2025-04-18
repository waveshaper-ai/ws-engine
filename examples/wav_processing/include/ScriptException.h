#ifndef _TL_SCRIPTEXCEPTION_H
#define _TL_SCRIPTEXCEPTION_H

#include "LibScriptExportOs.h"
#include "ComponentException.h"

namespace TL
{
namespace LibScript
{
/// Class to handle exceptions in LibScript.
class LIBSCRIPT_EXPORT ScriptException final : public LibCore::ComponentException
{
public:
    static constexpr char const* const LibScriptComp{"LibScriptComp"};

    enum class ExcType : u32
    {
        JsonInvalidValue,
        JsonInvalidTag,
        JsonInvalidType,
        JsonNotADocument,
        JsonNotAnObject,
        JsonNotAnArray,
        JsonCanNotOpenFile,
        JsonEmptyContent,
        NamedValueOutOfRange,
        NamedValueInvalidName
    };

    explicit ScriptException(ExcType type, std::string&& msg = "") noexcept;
    explicit ScriptException(std::string&& msg) noexcept;

    // Accesors
    inline ExcType type() const
    {
        return mType;
    }

    inline std::string const& getCompName() const { return LibCore::ComponentException::getCompName(); }

private:
    inline std::string getErrorMessage(ExcType type)
    {
        std::string msg;
        if(type == ExcType::NamedValueOutOfRange)
        {
            msg = "NamedValue out of validation range for";
        }
        return msg;
    }

private:
    ExcType mType;
};

} // namespace LibScript
} // namespace TL

#endif
