#ifndef TEXTURENODE_H_
#define TEXTURENODE_H_

#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/SamplerObject.h"

namespace he
{
  namespace renderer
  {
    class TextureNode
    {
    public:

      TextureNode();
      ~TextureNode();

      void initialize(const std::vector< std::vector<util::ResourceHandle> >& textureHandles);

      bool isTexture(std::vector< std::vector<util::ResourceHandle> >& textureHandles) const;

      const std::vector< std::vector<util::ResourceHandle> >& getTextureHandles() const;

      void bindTextures(util::SharedPointer<db::TextureManager> textureManager, std::vector<util::SharedPointer<SamplerObject>> samplerObjects) const;
      void unbindTextures(util::SharedPointer<db::TextureManager> textureManager, std::vector<util::SharedPointer<SamplerObject>> samplerObjects) const;

    private:

      std::vector< std::vector<util::ResourceHandle> > m_textureHandles;
    };
  }
}

#endif
