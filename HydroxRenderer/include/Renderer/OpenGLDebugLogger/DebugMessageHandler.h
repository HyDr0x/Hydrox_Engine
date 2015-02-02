#ifndef DEBUGMESSAGEHANDLER_H_
#define DEBUGMESSAGEHANDLER_H_

#include <iostream>
#include <string>
#include <GL/glew.h>

namespace he
{
  namespace renderer
  {
    #ifndef APIENTRY
      #define GLAPIENTRY __stdcall
    #endif

    static std::string getStringForType(GLenum type);
    static std::string getStringForSource(GLenum source);
    static std::string getStringForSeverity(GLenum severity);

    typedef void (GLAPIENTRY *GLDEBUGCALLBACK)(GLenum source,
                                                GLenum type,
                                                GLuint id,
                                                GLenum severity,
                                                GLsizei length,
                                                const GLchar* message,
                                                GLvoid* userParam);

    void GLAPIENTRY getDebugMessage(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar* message,
                                    GLvoid* userParam);
  }
}
#endif
