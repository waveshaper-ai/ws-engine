#ifndef _WS_FORMATPACK_H
#define _WS_FORMATPACK_H

#include "Constants.h"
#include "FormatException.h"
#include "FormatSpec.h"

#include <algorithm>
#include <bitset>
#include <chrono>
#include <climits>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>

namespace WS
{
namespace Util
{
namespace Format
{
template <class... Ts>
class FormatPack
{
public:
    FormatPack() = default;
    FormatPack(FormatPack const&) = delete;
    FormatPack(FormatPack&&) = delete;
    virtual ~FormatPack() = default;
    FormatPack& operator=(FormatPack const&) & = delete;
    FormatPack& operator=(FormatPack&&) & = delete;
};

template <class T, class... Ts>
class FormatPack<T, Ts...> : private FormatPack<Ts...>
{
public:
    FormatPack() = delete;
    FormatPack(FormatPack const&) = delete;
    FormatPack(FormatPack&&) = delete;
    ~FormatPack() override = default;
    explicit FormatPack(T&& t, Ts&&... ts) : FormatPack<Ts...>{std::forward<Ts>(ts)...}, mTail{t} {}
    FormatPack& operator=(FormatPack const&) & = delete;
    FormatPack& operator=(FormatPack&&) & = delete;

    FormatPack<Ts...> const& operator()() const
    {
        return *this;
    }

    T const& operator~() const
    {
        return mTail;
    }

private:
    T const& mTail;
};

template <class StrType, class... Ts>
std::string convertFormatPack(StrType const& fmtString, std::size_t offset, FormatPack<Ts...> const& fmtPack);

template <class StrType, class T, class... Ts>
std::string convertFormatPack(StrType const& fmtString, std::size_t const offset, FormatPack<T, Ts...> const& fmtPack)
{
    details::FormatSpecResult const foundSpec{details::findNextSpec(fmtString, offset)};
    if(!foundSpec.found())
    {
        throwFormatExcep("Need a format specifier since we have an argument");
    }

    std::string const beforeSpecifier(std::string(fmtString).substr(offset, foundSpec.startOffset() - offset));
    std::string const currentSpecifier(
        ConvertWithSpec<std::remove_cv_t<std::remove_reference_t<T>>>()(foundSpec.formatSpec(), ~fmtPack));
    std::string const remainingFormating(convertFormatPack(fmtString, foundSpec.endOffset() + 1U, fmtPack()));
    return details::cleanControllChar(beforeSpecifier) + currentSpecifier + remainingFormating;
}

template <class StrType>
std::string convertFormatPack(StrType const& fmtString, std::size_t const offset, FormatPack<> const&)
{
    details::FormatSpecResult const foundSpec{details::findNextSpec(fmtString, offset)};
    if(foundSpec.found())
    {
        throwFormatExcep("Should not have a format specifier since we do not have an argument");
    }
    return std::string(fmtString).substr(offset);
}

} // namespace Format
} // namespace Util
} // namespace WS

#endif // _WS_FORMATPACK_H
