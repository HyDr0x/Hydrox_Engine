#ifdef DLL_EXPORT
#define GRAPHICAPI __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define GRAPHICAPI __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif