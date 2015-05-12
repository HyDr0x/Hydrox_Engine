#ifndef TEXTURENODE_H_
#define TEXTURENODE_H_

#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/TreeNodes/GroupNode.h"

namespace he
{
  namespace renderer
  {
    class TextureNode : public GroupNode
    {
    public:

      TextureNode();
      ~TextureNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      bool preTraverse(ConstTraverser* traverser) const;
      void postTraverse(ConstTraverser* traverser) const;

      void initialize(std::vector< std::vector<util::ResourceHandle> >& textureHandles, util::vec4f uniColor);

      bool isTexture(std::vector< std::vector<util::ResourceHandle> >& textureHandles, util::vec4f uniColor) const;

      const std::vector< std::vector<util::ResourceHandle> >& getTextureHandles() const;

      util::vec4f getUniColor() const;

    private:

      util::vec4f m_uniColor;
      std::vector< std::vector<util::ResourceHandle> > m_textureHandles;
    };
  }
}

#endif
