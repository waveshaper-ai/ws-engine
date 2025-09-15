#define OS_WINDOWS 1

#ifdef BUILD_LIBUTIL
#ifdef __GNUC__
#define LIBUTIL_EXPORT __attribute__((dllexport))
#else
#define LIBUTIL_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define LIBUTIL_EXPORT __attribute__((dllimport))
#else
#define LIBUTIL_EXPORT __declspec(dllimport)
#endif
#endif

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#pragma warning(disable : 4244)

#define _NOEXCEPT noexcept
