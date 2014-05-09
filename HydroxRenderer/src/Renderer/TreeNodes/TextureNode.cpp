#include "Renderer/TreeNodes/TextureNode.h"

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Resources/Material.h"

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
      if(m_textureHandles[Material::DIFFUSETEX].size() != textureHandles[Material::DIFFUSETEX].size())
      {
        return false;
      }

      for(unsigned int j = 0; j < m_textureHandles[Material::DIFFUSETEX].size(); j++)
      {
        if(!(m_textureHandles[Material::DIFFUSETEX][j] == textureHandles[Material::DIFFUSETEX][j]))
        {
          return false;
        }
      }

      if(m_textureHandles[Material::NORMALTEX].size() != textureHandles[Material::NORMALTEX].size())
      {
        return false;
      }

      for(unsigned int j = 0; j < m_textureHandles[Material::NORMALTEX].size(); j++)
      {
        if(!(m_textureHandles[Material::NORMALTEX][j] == textureHandles[Material::NORMALTEX][j]))
        {
          return false;
        }
      }

      if(m_textureHandles[Material::SPECULARTEX].size() != textureHandles[Material::SPECULARTEX].size())
      {
        return false;
      }

      for(unsigned int j = 0; j < m_textureHandles[Material::SPECULARTEX].size(); j++)
      {
        if(!(m_textureHandles[Material::SPECULARTEX][j] == textureHandles[Material::SPECULARTEX][j]))
        {
          return false;
        }
      }

      if(m_textureHandles[Material::DISPLACEMENTTEX].size() != textureHandles[Material::DISPLACEMENTTEX].size())
      {
        return false;
      }

      for(unsigned int j = 0; j < m_textureHandles[Material::DISPLACEMENTTEX].size(); j++)
      {
        if(!(m_textureHandles[Material::DISPLACEMENTTEX][j] == textureHandles[Material::DISPLACEMENTTEX][j]))
        {
          return false;
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
