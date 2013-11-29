#include "Hydrox/Services/DebugLogManager.h"

#include <GL/glew.h>
#include <iostream>
#include <sstream>

DebugLogManager* DebugLogManager::getManager()
{
  static DebugLogManager* manager = new DebugLogManager();
  return manager;
}

DebugLogManager::DebugLogManager()
{
}

DebugLogManager::~DebugLogManager()
{
}

void DebugLogManager::gatherSystemInformation()
{
  m_vendorName = (const char*)glGetString(GL_VENDOR);
  m_graphicCardName = (const char*)glGetString(GL_RENDERER);
  m_shadingVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

  std::string openGLVersion = (const char*)glGetString(GL_VERSION);

  std::stringstream stream;
  stream << openGLVersion.substr(0, 1);
  stream >> m_openGLMajorVersion;

  std::stringstream stream2;
  stream2 << openGLVersion.substr(2, 3);
  stream2 >> m_openGLMinorVersion;

  for(GLuint i = 0; ; i++)
  {
    const char* extensionName = (const char*)glGetStringi(GL_EXTENSIONS, i);

    if(extensionName == nullptr)
    {
      break;
    }

    m_extensions += std::string(extensionName) + std::string("\n");

    i++;
  }
}

void DebugLogManager::printSystemInformation()
{
  std::cout << "Graphics Card Vendor: " << m_vendorName.c_str() << std::endl;
  std::cout << "Graphics Card: " << m_graphicCardName.c_str() << std::endl;
  std::cout << "OpenGL Version: " << m_openGLMajorVersion << "." << m_openGLMinorVersion << std::endl;
  std::cout << "Shader Version: " << m_shadingVersion.c_str() << std::endl;
}

void DebugLogManager::printSupportedExtensions()
{
  std::cout << "Extensions:\n" << m_extensions.c_str() << std::endl;
}

unsigned int DebugLogManager::getMajorOpenGLVersion()
{
  return m_openGLMajorVersion;
}

unsigned int DebugLogManager::getMinorOpenGLVersion()
{
  return m_openGLMinorVersion;
}