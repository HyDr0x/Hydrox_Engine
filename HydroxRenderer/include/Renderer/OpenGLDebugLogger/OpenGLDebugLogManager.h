#ifndef DEBUGLOGMANAGER_H_
#define DEBUGLOGMANAGER_H_

#include <string>

#include <GL/glew.h>

#include "Utilities/Miscellaneous/SingletonBehaviour.hpp"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI OpenGLDebugLogManager : public util::SingletonBehaviour
    {
    public:

      enum OpenGLCaps
      {
        MAX_IMAGE_UNITS,
        MAX_TEXTURE_UNITS,
        MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        MAX_TEXTURE_IMAGE_UNITS,
        MAX_COMBINED_TEXTURE_IMAGE_UNITS,

        MAX_VERTEX_SHADER_STORAGE_BLOCKS,
        MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
        MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
        MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
        MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
        MAX_COMPUTE_SHADER_STORAGE_BLOCKS,
        MAX_COMBINED_SHADER_STORAGE_BLOCKS,

        MAX_SHADER_STORAGE_BUFFER_BINDINGS,
        MAX_SHADER_STORAGE_BLOCK_SIZE,
        SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,

        MAX_COLOR_ATTACHMENTS,

        MAX_ARRAY_TEXTURE_LAYERS,

        OPENGLCAPSNUMBER,
      };

      OpenGLDebugLogManager();
      ~OpenGLDebugLogManager();

      void gatherSystemInformation();

      void initDebugMode(bool syncedOutput);

      void printSystemInformation() const;
      void printSupportedExtensions() const;

      unsigned int getMajorOpenGLVersion() const;
      unsigned int getMinorOpenGLVersion() const;

      void enableDebugMode() const;
      void disableDebugMode() const;

      void setEventSeverity(GLenum severity, bool enable) const;
      void setEventType(GLenum type, bool enable) const;
      void setEventSource(GLenum source, bool enable) const;

      void getState(GLenum pname, GLint *data) const;
      GLint getCapabilities(OpenGLCaps capability) const;

    private:

      bool m_syncedOutput;

      std::string m_vendorName;
      std::string m_graphicCardName;
      std::string m_shadingVersion;
      std::string m_extensions;

      unsigned int m_openGLMajorVersion;
      unsigned int m_openGLMinorVersion;

      GLint m_caps[OPENGLCAPSNUMBER];
    };
  }
}

#endif
