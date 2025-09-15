#ifndef _WS_STRFORMAT_H
#define _WS_STRFORMAT_H

#include "Constants.h"
#include "FormatPack.h"

#include <algorithm>
#include <bitset>

namespace WS
{
namespace Util
{
namespace Format
{
template <class StrType, class... Ts>
class StrFormat final
{
public:
    explicit StrFormat(StrType const& s, Ts&&... ts) : mFormatString{s}, mPack{std::forward<Ts>(ts)...} {}

    std::string operator()() const
    {
        return convertFormatPack<StrType, Ts...>(mFormatString, 0U, mPack);
    }

private:
    StrType const& mFormatString;
    FormatPack<Ts...> mPack;
};

template <class StrType, class... Ts>
inline std::string formatStr(StrType const& s, Ts&&... ts)
{
    return StrFormat<StrType, Ts...>(s, std::forward<Ts>(ts)...)();
}

} // namespace Format
} // namespace Util
} // namespace WS

#endif // _WS_STRFORMAT_H
