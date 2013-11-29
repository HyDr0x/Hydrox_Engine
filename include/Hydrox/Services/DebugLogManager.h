#ifndef DEBUGLOGMANAGER_H_
#define DEBUGLOGMANAGER_H_

#include <string>

#include "Hydrox/Services/Io_service.h"

#include "Hydrox/DLLExport.h"

namespace he
{
  class GRAPHICAPI DebugLogManager : public Io_service
  {
  public:

    static DebugLogManager* getManager();
    ~DebugLogManager();

    void gatherSystemInformation();

    void printSystemInformation();
    void printSupportedExtensions();

    unsigned int getMajorOpenGLVersion();
    unsigned int getMinorOpenGLVersion();

  private:

    DebugLogManager();

    std::string m_vendorName;
    std::string m_graphicCardName;
    std::string m_shadingVersion;
    std::string m_extensions;

    unsigned int m_openGLMajorVersion;
    unsigned int m_openGLMinorVersion;
  };
}

#endif