#include "Renderer/OpenGLDebugLogger/OpenGLDebugLogManager.h"

#include "Renderer/OpenGLDebugLogger/DebugMessageHandler.h"

#include <iostream>
#include <sstream>

namespace he
{
  namespace util
  {
    OpenGLDebugLogManager::OpenGLDebugLogManager()
    {
    }

    OpenGLDebugLogManager::~OpenGLDebugLogManager()
    {
    }

    void OpenGLDebugLogManager::gatherSystemInformation()
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

        m_extensions += std::string(extensionName) + std::string("/n");
      }

      glGetIntegerv(GL_MAX_TEXTURE_UNITS, &m_maxTextureUnits);
      glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &m_maxTextureVertexImageUnits);
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureImageUnits);
      glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_maxCombinedTextureUnits);
    }

    void OpenGLDebugLogManager::initDebugMode(bool syncedOutput) const
    {
      if(syncedOutput)
      {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      }
    
      GLDEBUGCALLBACK callback = &getDebugMessage;
      glDebugMessageCallback(callback, nullptr);
    }

    void OpenGLDebugLogManager::printSystemInformation() const
    {
      std::clog << "Graphics Card Vendor: " << m_vendorName.c_str() << std::endl;
      std::clog << "Graphics Card: " << m_graphicCardName.c_str() << std::endl;
      std::clog << "OpenGL Version: " << m_openGLMajorVersion << "." << m_openGLMinorVersion << std::endl;
      std::clog << "Shader Version: " << m_shadingVersion.c_str() << std::endl;
      std::clog << "---------------------------------------------------------" << std::endl;
    }

    void OpenGLDebugLogManager::printSupportedExtensions() const
    {
      std::clog << "Extensions:/n" << m_extensions.c_str() << std::endl;
    }

    unsigned int OpenGLDebugLogManager::getMajorOpenGLVersion() const
    {
      return m_openGLMajorVersion;
    }

    unsigned int OpenGLDebugLogManager::getMinorOpenGLVersion() const
    {
      return m_openGLMinorVersion;
    }

    void OpenGLDebugLogManager::enableDebugMode() const
    {
      glEnable(GL_DEBUG_OUTPUT);
    }

    void OpenGLDebugLogManager::disableDebugMode() const
    {
      glDisable(GL_DEBUG_OUTPUT);
    }

    void OpenGLDebugLogManager::setEventSeverity(GLenum severity, bool enable) const
    {
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, severity, 0, nullptr, enable);
    }

    void OpenGLDebugLogManager::setEventType(GLenum type, bool enable) const
    {
      glDebugMessageControl(GL_DONT_CARE, type, GL_DONT_CARE, 0, nullptr, enable);
    }

    void OpenGLDebugLogManager::setEventSource(GLenum source, bool enable) const
    {
      glDebugMessageControl(source, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, enable);
    }
  }
}