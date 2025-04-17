#ifdef BUILD_LIBCAFX
#ifdef __GNUC__
    #define LIB_CAFX_EXPORT __attribute__ ((dllexport))
#else
    #define LIB_CAFX_EXPORT  __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
    #define LIB_CAFX_EXPORT __attribute__ ((dllimport))
#else
    #define LIB_CAFX_EXPORT  __declspec(dllimport)
#endif
#endif