#ifndef _WS_EXCEPTION_H
#define _WS_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace WS
{
namespace Util
{
class Exception : public std::exception
{
public:
    std::string::value_type const* what() const noexcept final
    {
        return mMessage.c_str();
    }

protected:
    explicit Exception(std::string&& message) noexcept : std::exception{}
    {
        try
        {
            mMessage = std::move(message);
        }
        catch(std::exception const&)
        {
        }
    }
    Exception(Exception const&) = default;
    Exception(Exception&&) = default;
    Exception& operator=(Exception const&) & = default;
    Exception& operator=(Exception&&) & = default;

private:
    std::string mMessage{};
};

} // namespace Util
} // namespace WS

#endif // _WS_EXCEPTION_H
