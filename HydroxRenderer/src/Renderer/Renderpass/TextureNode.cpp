#include "Renderer/Renderpass/TextureNode.h"

#include "Renderer/Pipeline/SamplerObject.h"

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

    void TextureNode::initialize(const std::vector< std::vector<util::ResourceHandle> >& textureHandles)
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

    void TextureNode::bindTextures(util::SharedPointer<db::TextureManager> textureManager, std::vector<util::SharedPointer<SamplerObject>> samplerObjects) const
    {
      GLuint slotOffset = 0;

      for(unsigned int k = 0; k < m_textureHandles.size(); k++)
      {
        for(unsigned int l = 0; l < m_textureHandles[k].size(); l++)
        {
          db::Texture2D *texture = textureManager->getObject(m_textureHandles[k][l]);
          texture->setTexture(static_cast<GLint>(slotOffset + l), slotOffset + l);
          samplerObjects[k]->bindSampler(slotOffset + l);
        }

        slotOffset += static_cast<GLuint>(m_textureHandles[k].size());
      }
    }

    void TextureNode::unbindTextures(util::SharedPointer<db::TextureManager> textureManager, std::vector<util::SharedPointer<SamplerObject>> samplerObjects) const
    {
      GLuint slotOffset = 0;

      for(unsigned int k = 0; k < m_textureHandles.size(); k++)
      {
        for(unsigned int l = 0; l < m_textureHandles[k].size(); l++)
        {
          db::Texture2D *texture = textureManager->getObject(m_textureHandles[k][l]);
          texture->unsetTexture(slotOffset + l);
          samplerObjects[k]->unbindSampler(slotOffset + l);
        }

        slotOffset += static_cast<GLuint>(m_textureHandles[k].size());
      }
    }
  }
}
