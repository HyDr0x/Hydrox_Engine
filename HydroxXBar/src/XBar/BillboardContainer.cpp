#include "XBar/BillboardContainer.h"

namespace he
{
  namespace xBar
  {
    BillboardContainer::BillboardContainer(util::Vector<float, 3>& translation, 
                                          util::Vector<float, 2>& scale, 
                                          util::Vector<unsigned int, 2>& animNumber, 
                                          util::Vector<unsigned int, 2>& animCount, 
                                          util::Vector<float, 2>& texStart, 
                                          util::Vector<float, 2>& texEnd, 
                                          util::ResourceHandle textureHandle) : 
                                          m_animNumber(animNumber),
                                          m_animCount(animNumber),
                                          m_texStart(texStart),
                                          m_texEnd(texEnd),
                                          m_scale(scale),
                                          m_translate(translation),
                                          m_textureHandle(textureHandle)
    {
      createHash();
    }

    BillboardContainer::~BillboardContainer()
    {
    }

    bool BillboardContainer::operator == (const BillboardContainer& other) const
    {
      return m_hash == other.m_hash;
    }

    util::ResourceHandle BillboardContainer::getTextureHandle() const
    {
      return m_textureHandle;
    }

    util::Vector<float, 3> BillboardContainer::getPosition() const
    {
      return m_translate;
    }

    util::Vector<float, 2> BillboardContainer::getScale() const
    {
      return m_scale;
    }

    util::Matrix<float, 3> BillboardContainer::getTexTransformationMatrix() const
    {
      float width  = m_texEnd[0] - m_texStart[0];
      float height = m_texEnd[1] - m_texStart[1];

      return util::Matrix<float,3>(width/m_animNumber[0],0.0f,(static_cast<float>(m_animCount[0])/static_cast<float>(m_animNumber[0]))*width+m_texStart[0], 
                0.0f,height/m_animNumber[1],(static_cast<float>(m_animCount[1])/static_cast<float>(m_animNumber[1]))*height+m_texStart[1], 
                0.0f,0.0f,1.0f);
    }

    void BillboardContainer::createHash()
    {
      unsigned int id = m_textureHandle.getID();

      std::vector<unsigned char> data(sizeof(m_animNumber) + sizeof(m_animCount) + sizeof(m_texStart) + sizeof(m_texEnd) + sizeof(m_scale) + sizeof(m_translate) + sizeof(id));

      unsigned int offset = 0;
      offset = convertToRawData(m_animNumber, &data[0], offset);
      offset = convertToRawData(m_animCount, &data[0], offset);
      offset = convertToRawData(m_texStart, &data[0], offset);
      offset = convertToRawData(m_texEnd, &data[0], offset);
      offset = convertToRawData(m_scale, &data[0], offset);
      offset = convertToRawData(m_translate, &data[0], offset);
      offset = convertToRawData(id, &data[0], offset);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}