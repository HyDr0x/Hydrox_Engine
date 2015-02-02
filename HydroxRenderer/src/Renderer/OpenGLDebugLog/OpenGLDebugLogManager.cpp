#include "Renderer/OpenGLDebugLogger/OpenGLDebugLogManager.h"

#include "Renderer/OpenGLDebugLogger/DebugMessageHandler.h"

#include <iostream>
#include <sstream>

namespace he
{
  namespace renderer
  {
    OpenGLDebugLogManager::OpenGLDebugLogManager() : m_syncedOutput(false)
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

      for(GLuint i = 0; i < GLuint(maxExtensions); i++)
      {
        const char* extensionName = (const char*)glGetStringi(GL_EXTENSIONS, i);

        m_extensions += std::string(extensionName) + std::string("/n");
      }

      glGetIntegerv(GL_MAX_IMAGE_UNITS, &m_caps[MAX_IMAGE_UNITS]);
      glGetIntegerv(GL_MAX_TEXTURE_UNITS, &m_caps[MAX_TEXTURE_UNITS]);
      glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &m_caps[MAX_VERTEX_TEXTURE_IMAGE_UNITS]);
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_caps[MAX_TEXTURE_IMAGE_UNITS]);
      glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_caps[MAX_COMBINED_TEXTURE_IMAGE_UNITS]);
      glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &m_caps[MAX_VERTEX_SHADER_STORAGE_BLOCKS]);
      glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &m_caps[MAX_GEOMETRY_SHADER_STORAGE_BLOCKS]);
      glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &m_caps[MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS]);
      glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &m_caps[MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS]);
      glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &m_caps[MAX_FRAGMENT_SHADER_STORAGE_BLOCKS]);
      glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &m_caps[MAX_COMPUTE_SHADER_STORAGE_BLOCKS]);
      glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &m_caps[MAX_COMBINED_SHADER_STORAGE_BLOCKS]);
      glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &m_caps[MAX_SHADER_STORAGE_BUFFER_BINDINGS]);
      glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &m_caps[MAX_SHADER_STORAGE_BLOCK_SIZE]);
      glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &m_caps[SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT]);
      glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &m_caps[MAX_ARRAY_TEXTURE_LAYERS]);
      
      glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_caps[MAX_COLOR_ATTACHMENTS]);
    }

    void OpenGLDebugLogManager::initDebugMode(bool syncedOutput)
    {
      m_syncedOutput = syncedOutput;
      if(m_syncedOutput)
      {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      }

      glEnable(GL_DEBUG_OUTPUT);
    
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
      if(m_syncedOutput)
      {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      }

      glEnable(GL_DEBUG_OUTPUT);
    }

    void OpenGLDebugLogManager::disableDebugMode() const
    {
      if(m_syncedOutput)
      {
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      }

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

    void OpenGLDebugLogManager::getState(GLenum pname, GLint *data) const
    {
      glGetIntegerv(pname, data);
    }

    GLint OpenGLDebugLogManager::getCapabilities(OpenGLCaps capability) const
    {
      return m_caps[capability];
    }
  }
}