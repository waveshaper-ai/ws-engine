#define OS_DARWIN 1

#ifdef BUILD_LIBUTIL
#ifdef __GNUC__
#define LIBUTIL_EXPORT __attribute__((visibility("default")))
#else
#define LIBUTIL_EXPORT __attribute__((visibility("default")))
#endif
#else
#ifdef __GNUC__
#define LIBUTIL_EXPORT
#else
#define LIBUTIL_EXPORT
#endif
#endif
