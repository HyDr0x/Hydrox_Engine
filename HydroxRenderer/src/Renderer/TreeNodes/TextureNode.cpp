#include "Renderer/TreeNodes/TextureNode.h"

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/Traverser/ConstTraverser.h"

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

    bool TextureNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(this);
    }

    void TextureNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(this);
    }

    void TextureNode::initialize(std::vector< std::vector<util::ResourceHandle> >& textureHandles, util::vec4f uniColor)
    {
      m_uniColor = uniColor;
      m_textureHandles = textureHandles;
    }

    bool TextureNode::isTexture(std::vector< std::vector<util::ResourceHandle> >& textureHandles, util::vec4f uniColor) const
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

      return m_uniColor == uniColor;
    }

    const std::vector< std::vector<util::ResourceHandle> >& TextureNode::getTextureHandles() const
    {
      return m_textureHandles;
    }

    util::vec4f TextureNode::getUniColor() const
    {
      return m_uniColor;
    }
  }
}
