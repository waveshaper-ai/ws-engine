#ifdef BUILD_LIBSCRIPT
#ifdef __GNUC__
#define LIBSCRIPT_EXPORT __attribute__((dllexport))
#else
#define LIBSCRIPT_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define LIBSCRIPT_EXPORT __attribute__((dllimport))
#else
#define LIBSCRIPT_EXPORT __declspec(dllimport)
#endif
#endif

#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4244)
