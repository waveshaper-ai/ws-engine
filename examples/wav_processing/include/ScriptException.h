#ifndef _WS_SCRIPTEXCEPTION_H
#define _WS_SCRIPTEXCEPTION_H

#include "LibUtilExportOs.h"
#include "ComponentException.h"

namespace WS
{
namespace Util
{
/// Class to handle exceptions in Util.
class LIBUTIL_EXPORT ScriptException final : public Util::ComponentException
{
public:
    static constexpr char const* const LibUtilComp{"LibUtilComp"};

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

    inline std::string const& getCompName() const { return Util::ComponentException::getCompName(); }

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

} // namespace Util
} // namespace WS

#endif
