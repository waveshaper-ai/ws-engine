#ifndef _TL_FORMATSPEC_H
#define _TL_FORMATSPEC_H

#include "Constants.h"
#include "FormatException.h"

#include <algorithm>
#include <bitset>
#include <chrono>
#include <climits>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>

namespace TL
{
namespace LibCore
{
namespace Format
{
namespace details
{
constexpr std::string::value_type FormatSpecDot{'.'};
constexpr std::string::value_type FormatSpecColon{':'};
constexpr std::string::value_type FormatSpecStart{'{'};
constexpr std::string::value_type FormatSpecEnd{'}'};
constexpr std::string::value_type DefaultFillChar{'0'};

constexpr std::string::value_type FormatSpecBaseDecimal{'d'};
constexpr std::string::value_type FormatSpecBaseOctal{'o'};
constexpr std::string::value_type FormatSpecBaseBinary{'b'};
constexpr std::string::value_type FormatSpecBaseHexadecimal{'x'};
constexpr std::string::value_type FormatSpecBaseUpperHexadecimal{'X'};
} // namespace details

class FormatSpec final
{
public:
    enum class IntegerBase : u8
    {
        Decimal,
        Octal,
        Hexa,
        UpperHexa,
        Binary
    };

    template <class StrType>
    explicit FormatSpec(StrType const& s) : FormatSpec(std::begin(s), std::end(s))
    {
    }

    template <class StrIterType>
    FormatSpec(StrIterType const& sBegin, StrIterType const& sEnd)
    {
        if(sBegin == sEnd)
        {
            return;
        }

        StrIterType curIter{sBegin};
        // Handle the base format
        if((*curIter) == details::FormatSpecColon)
        {
            // Format is :[Width]DataType
            ++curIter;
            if(curIter == sEnd)
            {
                throwFormatExcep("Wrong format specifier for base: ", sBegin, sEnd);
            }

            StrIterType const endOfNumberIter{std::find_if_not(curIter, sEnd, &LibCore::Constants::isDigit)};
            if(curIter != endOfNumberIter)
            {
                mIntegerWidth.first = true;
                try
                {
                    mIntegerWidth.second = std::stoi(std::string(curIter, endOfNumberIter));
                }
                catch(std::exception const&)
                {
                    throwFormatExcep("Wrong format specifier for base: ", sBegin, sEnd);
                }
                curIter = endOfNumberIter;
            }

            if(curIter == sEnd)
            {
                throwFormatExcep("Wrong format specifier for base: ", sBegin, sEnd);
            }

            mIntegerBase.first = true;
            switch(*(curIter))
            {
            case details::FormatSpecBaseHexadecimal:
            {
                mIntegerBase.second = IntegerBase::Hexa;
                break;
            }
            case details::FormatSpecBaseUpperHexadecimal:
            {
                mIntegerBase.second = IntegerBase::UpperHexa;
                break;
            }
            case details::FormatSpecBaseOctal:
            {
                mIntegerBase.second = IntegerBase::Octal;
                break;
            }
            case details::FormatSpecBaseBinary:
            {
                mIntegerBase.second = IntegerBase::Binary;
                break;
            }
            case details::FormatSpecBaseDecimal:
            {
                mIntegerBase.second = IntegerBase::Decimal;
                break;
            }

            default:
            {
                std::string message{"Wrong format specifier for base."};
                throw FormatException{std::move(message)};
            }
            }
            ++curIter;
            if(curIter != sEnd)
            {
                throwFormatExcep("Wrong format specifier for base: ", sBegin, sEnd);
            }
        }
        // Handle the float fixed point
        else if((*curIter) == details::FormatSpecDot)
        {
            ++curIter;

            if(curIter == sEnd)
            {
                throwFormatExcep("Wrong format specifier for base: ", sBegin, sEnd);
            }

            try
            {
                mPrecision.first = true;
                mPrecision.second = std::stoi(std::string(curIter, sEnd));
            }
            catch(std::exception const&)
            {
                throwFormatExcep("Wrong format specifier for precision: ", sBegin, sEnd);
            }
        }
        else
        {
            throwFormatExcep("Unknown format specifier: ", sBegin, sEnd);
        }
    }

    // Default format spec.
    FormatSpec() = default;
    ~FormatSpec() = default;
    FormatSpec(FormatSpec const&) = default;
    FormatSpec(FormatSpec&&) = default;
    FormatSpec& operator=(FormatSpec const&) = default;
    FormatSpec& operator=(FormatSpec&&) = default;

    using PrecisionOption = std::pair<bool, s32>;
    using IntegerBaseOption = std::pair<bool, IntegerBase>;
    using IntegerWidthOption = std::pair<bool, s32>;

    PrecisionOption const& precisionOption() const
    {
        return mPrecision;
    }
    IntegerBaseOption const& integerBaseOption() const
    {
        return mIntegerBase;
    }
    IntegerWidthOption const& integerWidthOption() const
    {
        return mIntegerWidth;
    }

private:
    PrecisionOption mPrecision{false, {}};
    IntegerBaseOption mIntegerBase{false, IntegerBase::Decimal};
    IntegerWidthOption mIntegerWidth{false, 0};
};

namespace details
{
class FormatSpecResult final
{
public:
    FormatSpecResult() : mFound{false}, mFormatSpec{}, mStartOffset{}, mEndOffset{} {}
    FormatSpecResult(FormatSpec const& fmtSpec, std::size_t const start, std::size_t const end)
        : mFound{true}, mFormatSpec{fmtSpec}, mStartOffset{start}, mEndOffset{end}
    {
    }
    bool found() const
    {
        return mFound;
    }
    FormatSpec const& formatSpec() const
    {
        return mFormatSpec;
    }
    std::size_t startOffset() const
    {
        return mStartOffset;
    }
    std::size_t endOffset() const
    {
        return mEndOffset;
    }

private:
    bool mFound;
    FormatSpec mFormatSpec;
    std::size_t mStartOffset;
    std::size_t mEndOffset;
};

template <class StrType>
std::size_t FindElement(StrType const& haystack, std::string::value_type const needle, std::size_t const offset)
{
    using StrIterator = decltype(std::begin(haystack));
    StrIterator haystackSearchBegin{std::begin(haystack)};
    std::advance(haystackSearchBegin, offset);
    StrIterator const foundIter{std::find(haystackSearchBegin, std::end(haystack), needle)};
    if(foundIter == std::end(haystack))
    {
        return std::string::npos;
    }
    return static_cast<std::size_t>(std::distance(std::begin(haystack), foundIter));
}

template <class StrType>
std::size_t findStartSpec(StrType const& str, std::size_t const offset)
{
    using StrIterator = decltype(std::begin(str));
    using DistanceType = typename std::iterator_traits<StrIterator>::difference_type;

    StrIterator const begStr{std::next(std::begin(str), static_cast<DistanceType>(offset))};
    bool prevValueIsSpecStart{false};
    for(StrIterator cur{begStr}; cur != std::end(str); ++cur)
    {
        if(*cur == FormatSpecStart)
        {
            // Flip prevValueIsSpecStart:
            //  if it was false, now we have a spec start -> true
            //  if it was true, we have two spec start, so they cancel.
            prevValueIsSpecStart = !prevValueIsSpecStart;
        }
        else
        {
            if(prevValueIsSpecStart)
            {
                return static_cast<std::size_t>(std::distance(std::begin(str), std::prev(cur)));
            }
            else
            {
                prevValueIsSpecStart = false;
            }
        }
    }
    return std::string::npos;
}

template <class StrType>
FormatSpecResult findNextSpec(StrType const& s, std::size_t const offset)
{
    std::size_t const foundStartPos{findStartSpec(s, offset)};
    if(foundStartPos != std::string::npos)
    {
        std::size_t const foundEndPos{FindElement(s, FormatSpecEnd, foundStartPos)};
        if(foundEndPos != std::string::npos)
        {
            std::size_t const nextPosAfterStart{foundStartPos + 1U};
            return {FormatSpec(std::next(std::begin(s), nextPosAfterStart), std::next(std::begin(s), foundEndPos)),
                foundStartPos, foundEndPos};
        }
        else
        {
            throwFormatExcep("No end format delimiter");
        }
    }
    return {};
}
inline std::string cleanControllChar(std::string const& src)
{
    // remove consecutive controll characters
    std::string retVal{src};
    retVal.erase(std::string::const_iterator{std::unique(std::begin(retVal), std::end(retVal),
                     [](std::string::value_type const& c1, std::string::value_type const& c2) -> bool { return (c1 == FormatSpecStart) && (c1 == c2); })},
        std::cend(retVal));
    return retVal;
}

} // namespace details

template <typename T, typename Dummy = void>
class ConvertWithSpec final
{
public:
    std::string operator()(FormatSpec const&, T const& val) const
    {
        std::ostringstream os;
        os << val;
        return os.str();
    }
};

template <typename T>
class ConvertWithSpec<T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type>
    final
{
public:
    std::string operator()(FormatSpec const& formatSpec, T const& val) const
    {
        std::ostringstream os;
        if(formatSpec.integerBaseOption().first)
        {
            if(formatSpec.integerWidthOption().first)
            {
                os << std::setw(formatSpec.integerWidthOption().second) << std::setfill(details::DefaultFillChar);
            }

            switch(formatSpec.integerBaseOption().second)
            {
            case FormatSpec::IntegerBase::Binary:
            {
                constexpr char BitOn{'1'};
                constexpr char BitOff{'0'};
                constexpr u32 nbBitsInVal{Constants::BitsPerByte * sizeof(T)};
                constexpr u64 oneToShift{1U};
                u64 const castedVal{static_cast<u64>(val)};
                for(u32 bitIndex{0U}; bitIndex < nbBitsInVal; ++bitIndex)
                {
                    u32 const nbBitToShift{(nbBitsInVal - 1U) - bitIndex};
                    u64 const bitMask{oneToShift << nbBitToShift};

                    os << ((0U != (castedVal & bitMask)) ? BitOn : BitOff);
                }
                break;
            }
            case FormatSpec::IntegerBase::Decimal:
            {
                os << &std::dec << +val;
                break;
            }
            case FormatSpec::IntegerBase::Octal:
            {
                os << &std::oct << +val;
                break;
            }
            case FormatSpec::IntegerBase::Hexa:
            {
                os << &std::hex << +val;
                break;
            }
            case FormatSpec::IntegerBase::UpperHexa:
            {
                os << &std::uppercase << &std::hex << +val;
                break;
            }
            }
        }
        else
        {
            os << &std::dec << val;
        }

        return os.str();
    }
};

template <>
class ConvertWithSpec<bool, void> final
{
public:
    std::string operator()(FormatSpec const& fs, bool const& val) const
    {
        if(fs.integerBaseOption().first)
        {
            return (val ? "1" : "0");
        }
        else
        {
            return (val ? "true" : "false");
        }
    }
};

template <typename T>
class ConvertWithSpec<T, typename std::enable_if<!std::is_void<typename std::result_of<T()>::type>::value>::type> final
{
public:
    using Retype = typename std::result_of<T()>::type;
    std::string operator()(FormatSpec const& fs, T const& val) const
    {
        return ConvertWithSpec<Retype>()(fs, val());
    }
};

template <typename T>
class ConvertWithSpec<T, typename std::enable_if<std::is_enum<T>::value>::type> final
{
public:
    std::string operator()(FormatSpec const&, T const& val) const
    {
        std::ostringstream os;
        os << "Enum(" << std::to_string(+static_cast<typename std::underlying_type<T>::type>(val)) << ")";
        return os.str();
    }
};

template <typename T>
class ConvertWithSpec<T, typename std::enable_if<std::is_floating_point<T>::value>::type> final
{
public:
    std::string operator()(FormatSpec const& fs, T const& val) const
    {
        std::ostringstream os;
        if(fs.precisionOption().first)
        {
            os.setf(std::ios_base::fixed, std::ios_base::floatfield);
            os << std::setprecision(fs.precisionOption().second) << val;
        }
        else
        {
            os << val;
        }
        return os.str();
    }
};

template <typename T>
class ConvertWithSpec<T*, void> final
{
public:
    std::string operator()(FormatSpec const&, T const* const& val) const
    {
        std::ostringstream os;
        os << val;
        return os.str();
    }
};

template <typename T>
class ConvertWithSpec<std::unique_ptr<T>, void> final
{
public:
    std::string operator()(FormatSpec const& fs, std::unique_ptr<T> const& val) const
    {
        return ConvertWithSpec<T*>()(fs, val.get());
    }
};

template <>
class ConvertWithSpec<std::string, void> final
{
public:
    std::string operator()(FormatSpec const&, std::string const& val) const
    {
        return val;
    }
};

template <u32 N>
class ConvertWithSpec<char[N], void> final
{
public:
    std::string operator()(FormatSpec const&, const char (&val)[N]) const
    {
        return val;
    }
};

template <typename T>
class ConvertWithSpec<T, typename std::enable_if<std::is_base_of<std::exception, T>::value>::type> final
{
public:
    std::string operator()(FormatSpec const&, T const& val) const
    {
        return val.what();
    }
};

template <>
class ConvertWithSpec<std::chrono::system_clock::time_point, void> final
{
public:
    std::string operator()(FormatSpec const&, std::chrono::system_clock::time_point const& val) const
    {
        std::time_t const valInSeconds{std::chrono::system_clock::to_time_t(val)};

        std::ostringstream os;
        os << valInSeconds;
        return os.str();
    }
};

template <>
class ConvertWithSpec<std::chrono::milliseconds, void> final
{
public:
    std::string operator()(FormatSpec const&, std::chrono::milliseconds const& val) const
    {
        std::string str{std::to_string(val.count())};
        str += "ms";
        return str;
    }
};

} // namespace Format
} // namespace LibCore
} // namespace TL

#endif // _TL_FORMATSPEC_H
