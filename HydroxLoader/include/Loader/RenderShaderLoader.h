#ifndef RENDERSHADERLOADER_H_
#define RENDERSHADERLOADER_H_

#include <DataBase/Mesh.h>

#include "Loader/ShaderLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI RenderShaderLoader : public ShaderLoader
    {
    public:

      RenderShaderLoader();
      ~RenderShaderLoader();

      sh::RenderShader loadResource(sh::ShaderSlotFlags vertexDecaration,
                                        std::string filename,
                                        std::string vertexFilename, 
                                        std::string fragmentFilename = std::string(),
                                        std::string geometryFilename = std::string(),
                                        std::string tessControlFilename = std::string(),
                                        std::string tessEvalFilename = std::string());

      void loadShadersInIndexfile(std::string path, std::string shaderIndexFilename);

      virtual void checkIfShaderChanged();

    private:

      std::map<sh::RenderShaderHandle, std::vector<std::string>, sh::RenderShaderHandle::Less> m_shaderStageFilenames;
      std::map<std::string, std::vector<sh::RenderShaderHandle>> m_renderShaderMap;
    };
  }
}

#endif
