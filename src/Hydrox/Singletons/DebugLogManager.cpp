#include "Hydrox/Singletons/DebugLogManager.h"

#include "Hydrox/Singletons/DebugMessageHandler.h"

#include <iostream>
#include <sstream>

namespace he
{
  void DebugLogManager::createService()
  {
    m_instance = new DebugLogManager();
  }

  DebugLogManager::DebugLogManager()
  {
    /*if(m_debugMode)
    {
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);//looks like it doesn't work
    }
    else*/
    
    GLDEBUGCALLBACK callback = &getDebugMessage;
    glDebugMessageCallback(callback, nullptr);
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

    GLint maxExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &maxExtensions);

    for(GLuint i = 0; i < maxExtensions; i++)
    {
      const char* extensionName = (const char*)glGetStringi(GL_EXTENSIONS, i);

      m_extensions += std::string(extensionName) + std::string("\n");
    }

    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &m_maxTextureUnits);
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &m_maxTextureVertexImageUnits);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureImageUnits);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_maxCombinedTextureUnits);
  }

  void DebugLogManager::printSystemInformation()
  {
    std::cout << "Graphics Card Vendor: " << m_vendorName.c_str() << std::endl;
    std::cout << "Graphics Card: " << m_graphicCardName.c_str() << std::endl;
    std::cout << "OpenGL Version: " << m_openGLMajorVersion << "." << m_openGLMinorVersion << std::endl;
    std::cout << "Shader Version: " << m_shadingVersion.c_str() << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
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

  void DebugLogManager::enableDebugMode()
  {
    glEnable(GL_DEBUG_OUTPUT);
  }

  void DebugLogManager::disableDebugMode()
  {
    glDisable(GL_DEBUG_OUTPUT);
  }

  void DebugLogManager::setEventSeverity(GLenum severity, bool enable)
  {
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, severity, 0, nullptr, enable);
  }

  void DebugLogManager::setEventType(GLenum type, bool enable)
  {
    glDebugMessageControl(GL_DONT_CARE, type, GL_DONT_CARE, 0, nullptr, enable);
  }

  void DebugLogManager::setEventSource(GLenum source, bool enable)
  {
    glDebugMessageControl(source, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, enable);
  }
}