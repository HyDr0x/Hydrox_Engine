#if defined(_WIN32) || defined(__WIN32__)
  #ifdef DLL_EXPORT
    #define GRAPHICAPI __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define GRAPHICAPI __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #if __GNUC__ >= 4
    #define GRAPHICAPI __attribute__ ((__visibility__ ("default")))
    #define EXPIMP_TEMPLATE
  #else
    #define GRAPHICAPI __attribute__ ((__visibility__ ("default")))
    #define EXPIMP_TEMPLATE extern
  #endif
#endif