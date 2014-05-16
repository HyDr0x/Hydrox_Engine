#ifndef MATERIALLOADER_H_
#define MATERIALLOADER_H_

#include <map>

#include "Loader/ResourceLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI MaterialLoader : public ResourceLoader
    {
    public:

      enum MaterialFileKeywords
      {
        DEFAULT,
        DIFFUSESTRENGTH,
        SPECULARSTRENGTH,
        AMBIENTSTRENGTH,
        SPECULAREXPONENT,
        DIFFUSETEXTURE,
        NORMALMAP,
        SPECULARMAP,
        DISPLACEMENTMAP,
        SHADERNAME,
        FRAGMENTSHADER,
        GEOMETRYSHADER,
        TESSCONTROLSHADER,
        TESSEVALSHADER,
      };

      MaterialLoader(util::SingletonManager *singletonManager);
      ~MaterialLoader();

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource() const;

    private:

      std::map<std::string, MaterialFileKeywords> m_materialFileKeywords;

      renderer::MaterialManager *m_materialManager;
      renderer::TextureManager *m_textureManager;
      renderer::RenderShaderManager *m_renderShaderManager;
    };
  }
}

#endif
