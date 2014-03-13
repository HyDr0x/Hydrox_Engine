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
    }

    BillboardContainer::~BillboardContainer()
    {
    }

    BillboardContainer* BillboardContainer::clone()
    {
      return new BillboardContainer(m_translate, m_scale, m_animNumber, m_animCount, m_texStart, m_texEnd, m_textureHandle);
    }

    bool BillboardContainer::operator ==(const BillboardContainer& o)
    {
      return m_translate == o.m_translate && m_scale == o.m_scale && m_animNumber == o.m_animNumber && m_animCount == o.m_animCount && m_texStart == o.m_texStart && m_texEnd == o.m_texEnd && m_textureHandle == o.m_textureHandle;
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
  }
}