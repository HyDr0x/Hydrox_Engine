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
        TRANSPARENCY,
        RENDERDEBUG,
        UNICOLOR,
        DIFFUSETEXTURE,
        NORMALMAP,
        SPECULARMAP,
        DISPLACEMENTMAP,
      };

      MaterialLoader(util::SingletonManager *singletonManager);
      ~MaterialLoader();

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource() const;

    private:

      std::map<std::string, MaterialFileKeywords> m_materialFileKeywords;

      util::SharedPointer<db::MaterialManager> m_materialManager;
      util::SharedPointer<db::TextureManager> m_textureManager;
    };
  }
}

#endif
