#ifndef _WS_FORMATEXCEPTION_H
#define _WS_FORMATEXCEPTION_H

#include "Exception.h"

namespace WS
{
namespace Util
{
namespace Format
{
class FormatException final : public Exception
{
public:
    FormatException(std::string const& message, std::string const& argument) noexcept
        : Exception{std::string{message + argument}}
    {
    }

    explicit FormatException(std::string&& message) noexcept : Exception{std::move(message)} {}
};

template <class StrIterType>
inline void throwFormatExcep(std::string const& message, StrIterType const& sBegin, StrIterType const& sEnd)
{
    std::string exceptionArg;
    try
    {
        exceptionArg = std::string{sBegin, sEnd};
    }
    catch(std::exception const&)
    {
        // Something is wrong, exceptionArg will be empty, but throw it anyway..
    }
    throw FormatException{message, exceptionArg};
}
inline void throwFormatExcep(std::string&& message)
{
    throw FormatException{std::move(message)};
}

} // namespace Format
} // namespace Util
} // namespace WS

#endif // _WS_FORMATEXCEPTION_H
