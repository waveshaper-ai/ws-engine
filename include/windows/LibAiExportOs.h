#ifdef BUILD_LIBAI
#ifdef __GNUC__
    #define LIB_AI_EXPORT __attribute__ ((dllexport))
#else
    #define LIB_AI_EXPORT  __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
    #define LIB_AI_EXPORT __attribute__ ((dllimport))
#else
    #define LIB_AI_EXPORT  __declspec(dllimport)
#endif
#endif