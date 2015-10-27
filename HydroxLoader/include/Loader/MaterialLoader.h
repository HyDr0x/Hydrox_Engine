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
        METALNESS,
        REFLECTANCE,
        ROUGHNESS0,
        ROUGHNESS1,
        TRANSPARENCY,
        RENDERDEBUG,
        UNICOLOR,
        TEXTUREOFFSET,
        DIFFUSETEXTURE,
        NORMALMAP,
        METALNESSMAP,
        ROUGHNESSMAP,
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
