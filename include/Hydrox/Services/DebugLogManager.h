#ifndef DEBUGLOGMANAGER_H_
#define DEBUGLOGMANAGER_H_

#include <string>

#include "Hydrox/Services/Io_service.h"

#include "Hydrox/DLLExport.h"

class GRAPHICAPI DebugLogManager : public Io_service
{
public:

  DebugLogManager();
  ~DebugLogManager();

  void gatherSystemInformation();

  void printSystemInformation();
  void printSupportedExtensions();

  unsigned int getMajorOpenGLVersion();
  unsigned int getMinorOpenGLVersion();

private:

  std::string m_vendorName;
  std::string m_graphicCardName;
  std::string m_shadingVersion;
  std::string m_extensions;

  unsigned int m_openGLMajorVersion;
  unsigned int m_openGLMinorVersion;
};

#endif