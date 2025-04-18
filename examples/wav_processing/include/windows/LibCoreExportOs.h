#ifdef BUILD_LIBCORE
#ifdef __GNUC__
#define LIBCORE_EXPORT __attribute__((dllexport))
#else
#define LIBCORE_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define LIBCORE_EXPORT __attribute__((dllimport))
#else
#define LIBCORE_EXPORT __declspec(dllimport)
#endif
#endif
