#ifndef _WS_CONSTANT_H__
#define _WS_CONSTANT_H__

#include "BasicTypes.h"

namespace WS
{
namespace Util
{
namespace Constants
{
template <class T>
class Pi final
{
public:
    constexpr T operator()() const
    {
        constexpr T val{static_cast<T>(3.141'592'653'589'793'238'462'643'383'279'502'884)};
        return val;
    }
};

/// Time related constants
constexpr s64 NbMilisecondPerSecond{1000};
constexpr double NbMilisecondPerSecondDouble{static_cast<double>(NbMilisecondPerSecond)};

// Number base constants
constexpr u32 Base16{16U};
constexpr u32 BitsPerByte{8U};

// Characters
constexpr char underScoreCharacter{'_'};
constexpr char spaceCharacter{' '};
constexpr char doubleQuote{'\"'};
constexpr char colon{':'};
constexpr char tabCharacter{'\t'};

constexpr char openCurly{'{'};
constexpr char closeCurly{'}'};
constexpr char openSquare{'['};
constexpr char closeSquare{']'};

constexpr char carriageReturnChar{'\r'};
constexpr char newLineChar{'\n'};

constexpr char digitsFirst{'0'};
constexpr char digitsLast{'9'};

constexpr char lowerCaseAlphaFirst{'a'};
constexpr char lowerCaseAlphaLast{'z'};

constexpr char upperCaseAlphaFirst{'A'};
constexpr char upperCaseAlphaLast{'Z'};

constexpr bool isDigit(char const c)
{
    return (c >= digitsFirst) && (c <= digitsLast);
}
constexpr bool isLowerAlpha(char const c)
{
    return (c >= lowerCaseAlphaFirst) && (c <= lowerCaseAlphaLast);
}
constexpr bool isUpperAlpha(char const c)
{
    return (c >= upperCaseAlphaFirst) && (c <= upperCaseAlphaLast);
}
constexpr bool isAlpha(char const c)
{
    return isLowerAlpha(c) || isUpperAlpha(c);
}
constexpr bool isAlphaNumeric(char const c)
{
    return isAlpha(c) || isDigit(c);
}

} // namespace Constants
} // namespace Util
} // namespace WS

#endif //_WS_CONSTANT_H__
