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

    BillboardContainer* BillboardContainer::clone()
    {
      return new BillboardContainer(m_translate, m_scale, m_animNumber, m_animCount, m_texStart, m_texEnd, m_textureHandle);
    }

    util::ResourceHandle BillboardContainer::getTextureHandle() const
    {
      return m_textureHandle;
    }

    util::Vector<float, 3> BillboardContainer::getPosition()
    {
	    return m_translate;
    }

    util::Vector<float, 2> BillboardContainer::getScale()
    {
	    return m_scale;
    }

    util::Matrix<float, 3> BillboardContainer::getTexTransformationMatrix()
    {
      float width  = m_texEnd[0] - m_texStart[0];
	    float height = m_texEnd[1] - m_texStart[1];

      return util::Matrix<float,3>(width/m_animNumber[0],0.0f,(static_cast<float>(m_animCount[0])/static_cast<float>(m_animNumber[0]))*width+m_texStart[0], 
					      0.0f,height/m_animNumber[1],(static_cast<float>(m_animCount[1])/static_cast<float>(m_animNumber[1]))*height+m_texStart[1], 
					      0.0f,0.0f,1.0f);
    }

    void BillboardContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(void*) * 6 + sizeof(unsigned int));

      unsigned int id = m_textureHandle.getID();;

      memcpy(&data[0], &m_animNumber, sizeof(void*));
      memcpy(&data[sizeof(void*) * 1], &m_animCount, sizeof(void*));
      memcpy(&data[sizeof(void*) * 2], &m_texStart, sizeof(void*));
      memcpy(&data[sizeof(void*) * 3], &m_texEnd, sizeof(void*));
      memcpy(&data[sizeof(void*) * 4], &m_scale, sizeof(void*));
      memcpy(&data[sizeof(void*) * 5], &m_translate, sizeof(void*));
      memcpy(&data[sizeof(void*) * 6], &id, sizeof(unsigned int));

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}