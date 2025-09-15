#pragma once

#include "BasicTypes.h"
#include "LibUtilExportOs.h"
#include <mutex>
#include <string>
#include <vector>

typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

namespace WS
{
namespace Util
{
using DataBuffer = std::vector<u8>;

/// Utility methods to easily convert from a string to an encoded buffer, back and forth.
LIBUTIL_EXPORT DataBuffer to_buffer(std::string const& str);
LIBUTIL_EXPORT std::string to_string(DataBuffer const& buffer);
LIBUTIL_EXPORT DataBuffer decrypt(DataBuffer const& input);
LIBUTIL_EXPORT DataBuffer encrypt(DataBuffer const& input);

} // namespace Util
} // namespace WS
