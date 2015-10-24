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

      sh::RenderShader loadResource(const std::string& filename,
                                    const std::vector<std::string>& shaderFilenames);

      void loadShadersInIndexfile(std::string path, std::string shaderIndexFilename);

      virtual void checkIfShaderChanged();

    private:

      sh::ShaderSlotFlags readVertexDeclaration(const std::string& vertexShaderSource);
      void registerShaderSourceFiles(const std::string& shaderFilename, sh::RenderShaderHandle shaderHandle);

      std::map<std::string, std::vector<sh::RenderShaderHandle>> m_renderShaderMap;
    };
  }
}

#endif
