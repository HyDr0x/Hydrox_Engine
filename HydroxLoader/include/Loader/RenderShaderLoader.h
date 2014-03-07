#ifndef RENDERSHADERLOADER_H_
#define RENDERSHADERLOADER_H_

#include "Loader/ShaderLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI RenderShaderLoader : public ShaderLoader
    {
    public:

      RenderShaderLoader(util::SingletonManager *singletonManager);
      ~RenderShaderLoader();

      void setUsedShaderPrograms(bool fragmentShaderProgram,
                                 bool geometryShaderProgram,
                                 bool tesselationControlShaderProgram,
                                 bool tesselationEvalShaderProgram);

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource();

    private:

      bool m_fragmentShaderProgram;
      bool m_geometryShaderProgram;
      bool m_tesselationControlShaderProgram;
      bool m_tesselationEvalShaderProgram;

      renderer::RenderShaderManager *m_renderShaderManager;
    };
  }
}

#endif
