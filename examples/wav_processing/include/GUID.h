#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <string>

namespace WS
{
namespace Util
{
class LIBUTIL_EXPORT GUID
{

public:
    GUID();
    explicit GUID(bool generate);
    GUID(const GUID& rhs);
    GUID(const char* guidStr, bool useMicrosoftFormat = false);
    GUID(const std::string& guidStr, bool useMicrosoftFormat = false);
    ~GUID();

    GUID& operator=(const GUID& rhs);
    GUID& operator=(u8*);
    void copyToArray(u8 array[16]) const;

    bool operator==(const GUID& rhs) const;
    bool operator!=(const GUID& rhs) const;
    bool operator<(const GUID& rhs) const;
    bool operator>(const GUID& rhs) const;
    inline operator const u8*() const;
    inline operator u8*();
    inline operator const std::string() const;

    /// Returns the number of bytes to store a GUID
    inline u32 getSize() const;

    void getGUIDasString(std::string& guidStrToFill, bool useMicrosoftFormat = false) const;

private:
    void generate();

    // Data Members
protected:
    // ulong windows = 32bits,
    // ulong everywhere else (Mac OS/Linux) = 64bits, uint = 32bits
    // The other types are the same
    u32 _data1;
    u16 _data2;
    u16 _data3;
    u8 _data4[8];
};

//=============================================================================
// I N L I N E   F U N C T I O N S

//-----------------------------------------------------------------------------
//
inline GUID::operator const u8*() const
{
    return reinterpret_cast<const u8*>(&_data1);
}

//-----------------------------------------------------------------------------
//
inline GUID::operator const std::string() const
{
    std::string guidStr;
    getGUIDasString(guidStr, true);
    return guidStr;
}

//-----------------------------------------------------------------------------
//
inline GUID::operator u8*()
{
    return reinterpret_cast<u8*>(&_data1);
}

//-----------------------------------------------------------------------------
//
inline u32
GUID::getSize() const
{
    return sizeof(GUID);
}

} // namespace Util
} // namespace WS