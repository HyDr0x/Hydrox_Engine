#include "Renderer/Resources/Material.h"

namespace he
{
	namespace renderer
	{
	

    Material::Material()
    {
    }

    Material::Material(MaterialData& materialData, std::vector< std::vector<util::ResourceHandle> >& textureIndices, util::ResourceHandle shader)
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

      m_shaderHandle = shader;
      m_textureHandles = textureIndices;
      m_materialData = materialData;
    }

    Material::Material(const Material& o)
    { 
      m_hash = o.m_hash;

      m_materialData = o.m_materialData; 

      m_textureHandles = o.m_textureHandles;
      m_shaderHandle = o.m_shaderHandle;
    }

    Material& Material::operator=(const Material& o)
    {
      m_hash = o.m_hash;

      m_materialData = o.m_materialData; 

      m_textureHandles = o.m_textureHandles;
      m_shaderHandle = o.m_shaderHandle;

      return *this;
    }

    Material::~Material()
    { 
    }

    void Material::free()
    {
      m_shaderHandle.free();

      for(int i = 0; i < m_textureHandles.size(); i++)
      {
	      for(int j = 0; j < m_textureHandles[i].size(); j++)
        {
          m_textureHandles[i][j].free();
        }
      }
    }

    unsigned int Material::getTextureNumber(TextureType texType) const
    {
	    assert(texType >= 0 && texType < m_textureHandles.size());
	    return static_cast<unsigned int>(m_textureHandles[texType].size());
    }

    void Material::setTextureNumber(TextureType texType, unsigned int texNum)
    {
      assert(texType >= 0 && texType < m_textureHandles.size());
      m_textureHandles[texType].resize(texNum);
    }

    void Material::setTextureHandle(TextureType texType, unsigned int slot, util::ResourceHandle textureHandle)
    {
      assert(texType >= 0 && texType < m_textureHandles.size() && slot >= 0 && slot < m_textureHandles[texType].size());

      m_textureHandles[texType][slot] = textureHandle;
    }

    util::ResourceHandle Material::getTextureHandle(TextureType texType, unsigned int slot) const
    {
      assert(texType >= 0 && texType < m_textureHandles.size() && slot >= 0 && slot < m_textureHandles[texType].size());

      return m_textureHandles[texType][slot];
    }

    void Material::setShaderHandle(util::ResourceHandle shaderHandle)
    {
      m_shaderHandle = shaderHandle;
    }

    util::ResourceHandle Material::getShaderHandle() const
    {
      return m_shaderHandle;
    }

    void Material::setMaterialData(Material::MaterialData& material)
    {
      m_materialData = material;
    }

    Material::MaterialData& Material::getMaterialData()
    {
      return m_materialData;
    }

	}
}