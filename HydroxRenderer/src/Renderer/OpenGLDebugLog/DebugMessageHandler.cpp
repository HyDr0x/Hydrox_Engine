#include "Renderer/OpenGLDebugLogger/DebugMessageHandler.h"

namespace he
{
  namespace util
  {
    void GLAPIENTRY getDebugMessage(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar* message,
                                    GLvoid* userParam)
    {
      std::cerr << "Source: " << getStringForSource(source) << std::endl;
      std::cerr << "Type: " << getStringForType(type) << std::endl;
      std::cerr << "Severity: " << getStringForSeverity(severity) << std::endl;
      std::cerr << message << std::endl;
      std::cerr << "---------------------------------------------------------" << std::endl;
      std::cerr << std::endl;
    }

    std::string getStringForType(GLenum type)
    {
      switch(type)
      {
        case GL_DEBUG_TYPE_ERROR:
          return std::string("Event error");
          break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
          return std::string("Deprecated behaviour");
          break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
          return std::string("Undefined behaviour");
          break;
        case GL_DEBUG_TYPE_MARKER:
          return std::string("Command stream annotation");
          break;
        case GL_DEBUG_TYPE_PERFORMANCE:
          return std::string("Implementation-dependent performance warning");
          break;
        case GL_DEBUG_TYPE_PORTABILITY:
          return std::string("Highly vendor-specific extension");
          break;
        case GL_DEBUG_TYPE_POP_GROUP:
          return std::string("Leaving debug group");
          break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
          return std::string("Entering debug group");
          break;
        case GL_DEBUG_TYPE_OTHER:
          return std::string("Other event type");
          break;
        default:
          return std::string("UNEXPECTED TYPE");
      }
    }

    std::string getStringForSource(GLenum source)
    {
      switch(source)
      {
        case GL_DEBUG_SOURCE_API:
          return std::string("OpenGL");
          break;
        case GL_DEBUG_SOURCE_APPLICATION:
          return std::string("The application");
          break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
          return std::string("External debuggers or third-party middleware libraries");
          break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
          return std::string("The GLSL shader compiler or compilers for other extension-provided languages");
          break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
          return std::string("The window system (eg. WGL or GLX)");
          break;
        case GL_DEBUG_SOURCE_OTHER:
          return std::string("Other source");
          break;
        default:
          return std::string("UNEXPECTED SOURCE");
      }
    }

    std::string getStringForSeverity(GLenum severity)
    {
      switch(severity)
      {
        case GL_DEBUG_SEVERITY_HIGH:
          return std::string("HIGH");
          break;
        case GL_DEBUG_SEVERITY_MEDIUM:
          return std::string("MEDIUM");
          break;
        case GL_DEBUG_SEVERITY_LOW:
          return std::string("LOW");
          break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
          return std::string("NOTIFICATION");
          break;
        default:
          return std::string("UNEXPECTED SEVERITY");
      }
    }
  }
}
