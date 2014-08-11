#include "DataBase/Material.h"

namespace he
{
  namespace db
  {
    Material::Material()
    {
    }

    Material::Material(MaterialData& materialData, const std::vector< std::vector<util::ResourceHandle> >& textureIndices, util::ResourceHandle shader, bool transparency) : m_transparency(transparency)
    {
      unsigned int length = sizeof(MaterialData) + sizeof(unsigned int);
      std::vector<unsigned char> data(length);
      std::copy(&materialData, &materialData + 1, (MaterialData*)&data[0]);
      unsigned int id = shader.getID();
      std::copy(&id, &id + 1, &data[sizeof(MaterialData)]);

      for(size_t i = 0; i < textureIndices.size(); i++)
      {
        for(size_t j = 0; j < textureIndices[i].size(); j++)
        {
          data.resize(data.size() + sizeof(unsigned int));
          id = textureIndices[i][j].getID();
          std::copy(&id, &id + 1, &data[length]);
          length += sizeof(unsigned int);
        }
      }

      m_hash = MurmurHash64A(&data[0], length, 0);

      m_shaderHandle = shader;
      m_textureHandles = textureIndices;
      m_materialData = materialData;
    }

    Material::Material(const Material& other)
    { 
      m_hash = other.m_hash;

      m_materialData = other.m_materialData; 

      m_textureHandles = other.m_textureHandles;
      m_shaderHandle = other.m_shaderHandle;
    }

    Material::~Material()
    { 
    }

    //copy swap idiom
    Material& Material::operator=(Material other)//CTor of other called for auto construction
    {
      swap(other);//swap both

      return *this;
    }//DTor of other called, for auto cleaning

    void Material::swap(Material& other)
    {
      std::swap(m_hash, other.m_hash);
      std::swap(m_materialData, other.m_materialData);
      std::swap(m_textureHandles, other.m_textureHandles);
      std::swap(m_shaderHandle, other.m_shaderHandle);
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

    void Material::setMaterialData(const Material::MaterialData& material)
    {
      m_materialData = material;
    }

    const Material::MaterialData& Material::getMaterialData() const
    {
      return m_materialData;
    }

    bool Material::getTransparency() const
    {
      return m_transparency;
    }
  }
}