#include "Hydrox/Graphics/Material.h"

namespace he
{
  Material::Material()
  {
  }

  Material::Material(MaterialData& materialData, std::vector< std::vector<ResourceHandle> >& textureIndices, ResourceHandle shader)
  {
    unsigned int length = sizeof(MaterialData) + sizeof(unsigned int);
    std::vector<unsigned char> data(length);
    memcpy(&data[0], &materialData, sizeof(MaterialData));
    unsigned int id = shader.getID();
    memcpy(&data[sizeof(MaterialData)], &id, sizeof(unsigned int));

    for(size_t i = 0; i < textureIndices.size(); i++)
    {
      for(size_t j = 0; j < textureIndices[i].size(); j++)
      {
        data.resize(data.size() + sizeof(unsigned int));
        unsigned int id = textureIndices[i][j].getID();
        memcpy(&data[length], &id, sizeof(unsigned int));
        length += sizeof(unsigned int);
      }
    }

    m_hash = MurmurHash64A(&data[0], length, 0);

    m_shaderIndex = shader;
    m_textureIndices = textureIndices;
    m_materialData = materialData;
    //for(int i = 0; i!= textures.size(); i++)
    //{
    //  for(int j = 0; j!= textures[i].size(); j++)
    //  {
    //    m_textureIndices[i][j] = SharedPointer<Texture>(textures[i][j]);
    //  }
    //}
  }

  Material::Material(const Material& o)
  { 
    m_hash = o.m_hash;

    m_materialData = MaterialData(o.m_materialData.diffuseStrength, 
                                  o.m_materialData.specularStrength, 
                                  o.m_materialData.ambientStrength, 
                                  o.m_materialData.specularExponent, 
                                  o.m_materialData.transparency); 
  }

  Material& Material::operator=(const Material& o)
  {
    m_hash = o.m_hash;

    m_materialData = MaterialData(o.m_materialData.diffuseStrength, 
                                  o.m_materialData.specularStrength, 
                                  o.m_materialData.ambientStrength, 
                                  o.m_materialData.specularExponent, 
                                  o.m_materialData.transparency); 

    m_textureIndices = o.m_textureIndices;
    m_shaderIndex = o.m_shaderIndex;//specifies the Shader of the submesh in the Shadermanager for the renderpass

    return *this;
  }

  Material::~Material()
  { 
    free();
  }

  void Material::free()
  {
    if(m_shaderIndex.getID() != ~0)
    {
      m_shaderIndex.free();
    }

    for(int i = 0; i < m_textureIndices.size(); i++)
    {
	    for(int j = 0; j < m_textureIndices[i].size(); j++)
      {
        if(m_textureIndices[i][j].getID() != ~0)
        {
          m_textureIndices[i][j].free();
        }
      }
    }
  }

  unsigned int Material::getTextureNumber(TextureType texType) const
  {
	  assert(texType >= 0 && texType < m_textureIndices.size());
	  return static_cast<unsigned int>(m_textureIndices[texType].size());
  }

  void Material::setTextureNumber(TextureType texType, unsigned int texNum)
  {
    assert(texType >= 0 && texType < m_textureIndices.size());
    m_textureIndices[texType].resize(texNum);
  }

  void Material::setTexture(TextureType texType, unsigned int slot, ResourceHandle textureIndex)
  {
    assert(texType >= 0 && texType < m_textureIndices.size() && slot >= 0 && slot < m_textureIndices[texType].size());

    m_textureIndices[texType][slot] = textureIndex;
  }

  ResourceHandle Material::getTexture(TextureType texType, unsigned int slot) const
  {
    assert(texType >= 0 && texType < m_textureIndices.size() && slot >= 0 && slot < m_textureIndices[texType].size());

    return m_textureIndices[texType][slot];
  }

  void Material::setShader(ResourceHandle shaderIndex)
  {
    m_shaderIndex = shaderIndex;
  }

  ResourceHandle Material::getShader() const
  {
    return m_shaderIndex;
  }

  void Material::setMaterial(Material::MaterialData& material)
  {
    m_materialData = material;
  }

  const Material::MaterialData& Material::getMaterial() const
  {
    return m_materialData;
  }
}