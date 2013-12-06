#ifndef DEBUGLOGMANAGER_H_
#define DEBUGLOGMANAGER_H_

#include <string>

#include <GL/glew.h>

#include "Hydrox/Services/Io_service.h"

#include "Hydrox/DLLExport.h"

namespace he
{
  class GRAPHICAPI DebugLogManager : public Io_service
  {
  public:

    static DebugLogManager* getManager(bool debugMode);
    ~DebugLogManager();

    void gatherSystemInformation();

    void printSystemInformation();
    void printSupportedExtensions();

    unsigned int getMajorOpenGLVersion();
    unsigned int getMinorOpenGLVersion();

    void enableDebugMode();
    void disableDebugMode();

    void setEventSeverity(GLenum severity, bool enable);
    void setEventType(GLenum type, bool enable);
    void setEventSource(GLenum source, bool enable);

  private:

    DebugLogManager(bool debugMode);

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

#endif