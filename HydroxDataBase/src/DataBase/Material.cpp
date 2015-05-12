#include "DataBase/Material.h"

namespace he
{
  namespace db
  {
    Material::Material()
    {
    }

    Material::Material(const MaterialData& materialData, const std::vector< std::vector<util::ResourceHandle> >& textureHandles, std::vector< std::vector<uint64_t> > textureHashes, bool transparency, bool debug, util::vec4f uniColor) :
      m_transparency(transparency),
      m_debug(debug),
      m_uniColor(uniColor),
      m_textureHashes(textureHashes),
      m_textureHandles(textureHandles),
      m_materialData(materialData)
    {
    }

    Material::Material(const Material& other)
    { 
      m_materialData = other.m_materialData; 

      m_transparency = other.m_transparency;
      m_debug = other.m_debug;
      m_uniColor = other.m_uniColor;

      m_textureHashes = other.m_textureHashes;
      m_textureHandles = other.m_textureHandles;
    }

    Material::~Material()
    { 
    }

    //copy swap idiom
    Material& Material::operator=(Material other)//CTor of other called for auto construction
    {
      using namespace std;//for ADL
      swap(other);//swap both

      return *this;
    }//DTor of other called, for auto cleaning

    void Material::swap(Material& other)
    {
      std::swap(m_hash, other.m_hash);
      std::swap(m_materialData, other.m_materialData);
      std::swap(m_transparency, other.m_transparency);
      std::swap(m_debug, other.m_debug);
      std::swap(m_uniColor, other.m_uniColor);
      std::swap(m_textureHashes, other.m_textureHashes);
      std::swap(m_textureHandles, other.m_textureHandles);
    }

    void Material::free()
    {
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

    void Material::setTextureHandle(TextureType texType, unsigned int slot, util::ResourceHandle textureHandle, uint64_t textureHash)
    {
      assert(texType >= 0 && texType < m_textureHandles.size() && slot >= 0 && slot < m_textureHandles[texType].size());

      m_textureHandles[texType][slot] = textureHandle;
      m_textureHashes[texType][slot] = textureHash;

      m_dirtyHash = true;
    }

    util::ResourceHandle Material::getTextureHandle(TextureType texType, unsigned int slot) const
    {
      assert(texType >= 0 && texType < m_textureHandles.size() && slot >= 0 && slot < m_textureHandles[texType].size());

      return m_textureHandles[texType][slot];
    }

    std::vector< std::vector<util::ResourceHandle> > Material::getTextureHandles() const
    {
      return m_textureHandles;
    }

    bool Material::equalTextureHandles(const std::vector<std::vector<util::ResourceHandle>>& textureHandles) const
    {
      bool inserted = true;

      for(unsigned int j = 0; j < textureHandles.size(); j++)
      {
        for(unsigned int k = 0; k < textureHandles[j].size(); k++)
        {
          if(textureHandles[j][k] != m_textureHandles[j][k])
          {
            inserted = false;
            break;
          }
        }
      }

      return inserted;
    }

    void Material::setMaterialData(const Material::MaterialData& material)
    {
      m_materialData = material;

      m_dirtyHash = true;
    }

    const Material::MaterialData& Material::getMaterialData() const
    {
      return m_materialData;
    }

    bool Material::getTransparency() const
    {
      return m_transparency;
    }

    bool Material::getDebug() const
    {
      return m_debug;
    }

    util::vec4f Material::getUniColor() const
    {
      return m_uniColor;
    }

    void Material::setUniColor(util::vec4f uniColor)
    {
      m_uniColor = uniColor;

      m_dirtyHash = true;
    }

    void Material::updateHash()
    {
      unsigned int length = sizeof(MaterialData) + sizeof(m_transparency) + sizeof(m_debug) + sizeof(m_uniColor);

      for(unsigned int i = 0; i < TEXTURETYPENUM; i++)
      {
        length += sizeof(uint64_t) * m_textureHashes[i].size();
      }

      std::vector<unsigned char> data(length);
      std::copy(&m_materialData, &m_materialData + 1, (MaterialData*)&data[0]);
      std::copy(&m_transparency, &m_transparency + 1, (bool*)(&data[0] + sizeof(MaterialData)));
      std::copy(&m_debug, &m_debug + 1, (bool*)(&data[0] + sizeof(MaterialData) + sizeof(bool)));
      std::copy(&m_uniColor, &m_uniColor + 1, (util::vec4f*)(&data[0] + sizeof(MaterialData) + sizeof(bool) + sizeof(bool)));

      unsigned int offset = sizeof(MaterialData) + sizeof(bool) + sizeof(bool) + sizeof(util::vec4f);
      for(unsigned int i = 0; i < TEXTURETYPENUM; i++)
      {
        if(!m_textureHashes[i].empty())
        {
          std::copy(&m_textureHashes[i][0], &m_textureHashes[i][0] + m_textureHashes[i].size(), (uint64_t*)&data[offset]);
          offset += sizeof(uint64_t) * m_textureHashes[i].size();
        }
      }

      m_hash = MurmurHash64A(&data[0], length, 0);
    }
  }
}