#include "Renderer/TreeNodes/TextureNode.h"

#include "Renderer/Traverser/Traverser.h"

#include <DataBase/Material.h>

namespace he
{
  namespace renderer
  {
    TextureNode::TextureNode() 
    {
    }

    TextureNode::~TextureNode()
    {
    }

    bool TextureNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void TextureNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    void TextureNode::initialize(std::vector< std::vector<util::ResourceHandle> >& textureHandles)
    {
      m_textureHandles = textureHandles;
    }

    bool TextureNode::isTexture(std::vector< std::vector<util::ResourceHandle> >& textureHandles) const
    {
      for(unsigned int i = 0; i < m_textureHandles.size(); i++)
      {
        if(m_textureHandles[i].size() != textureHandles[i].size())
        {
          return false;
        }

        for(unsigned int j = 0; j < m_textureHandles[i].size(); j++)
        {
          if(!(m_textureHandles[i][j] == textureHandles[i][j]))
          {
            return false;
          }
        }
      }

      return true;
    }

    const std::vector< std::vector<util::ResourceHandle> >& TextureNode::getTextureHandles() const
    {
      return m_textureHandles;
    }
  }
}
