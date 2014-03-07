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
    class Traverser;

    class TextureNode : public GroupNode
    {
    public:

      TextureNode();
      ~TextureNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      void initialize(std::vector< std::vector<util::ResourceHandle> >& textureHandles);

      bool isTexture(std::vector< std::vector<util::ResourceHandle> >& textureHandles);

      std::vector< std::vector<util::ResourceHandle> >& getTextureHandles();

    private:

      std::vector< std::vector<util::ResourceHandle> > m_textureHandles;
    };
	}
}

#endif
