#ifndef DEBUGLOGMANAGER_H_
#define DEBUGLOGMANAGER_H_

#include <string>

#include <GL/glew.h>

#include "Utilities/Miscellaneous/SingletonBehaviour.hpp"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI OpenGLDebugLogManager : public SingletonBehaviour
    {
    public:

      OpenGLDebugLogManager();
      ~OpenGLDebugLogManager();

      void gatherSystemInformation();

      void initDebugMode() const;

      void printSystemInformation() const;
      void printSupportedExtensions() const;

      unsigned int getMajorOpenGLVersion() const;
      unsigned int getMinorOpenGLVersion() const;

      void enableDebugMode() const;
      void disableDebugMode() const;

      void setEventSeverity(GLenum severity, bool enable) const;
      void setEventType(GLenum type, bool enable) const;
      void setEventSource(GLenum source, bool enable) const;

    private:

      std::string m_vendorName;
      std::string m_graphicCardName;
      std::string m_shadingVersion;
      std::string m_extensions;

      unsigned int m_openGLMajorVersion;
      unsigned int m_openGLMinorVersion;

      GLint m_maxTextureUnits;
      GLint m_maxTextureVertexImageUnits;
      GLint m_maxTextureImageUnits;
      GLint m_maxCombinedTextureUnits;
    };
  }
}

#endif
