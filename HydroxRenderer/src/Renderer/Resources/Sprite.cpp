#include "Renderer/Resources/Sprite.h"

#include <vector>

namespace he
{
	namespace renderer
	{
	

    Sprite::Sprite(util::ResourceHandle texID, bool renderable, util::Vector<unsigned int, 2> animNumber, util::Vector<float, 2> texStart, util::Vector<float, 2> texEnd) : m_texID(texID), 
                                                                                                                                                    m_renderable(renderable), 
                                                                                                                                                    m_animNumber(animNumber), 
                                                                                                                                                    m_animCount(util::Vector<unsigned int, 2>(0, 0)),
                                                                                                                                                    m_texStart(texStart),
                                                                                                                                                    m_texEnd(texEnd),
                                                                                                                                                    m_angle(0.0f),
                                                                                                                                                    m_rtMatrix(util::Matrix<float, 3>::identity()),
                                                                                                                                                    m_tlMatrix(util::Matrix<float, 3>::identity()),
                                                                                                                                                    m_scMatrix(util::Matrix<float, 3>::identity()),
                                                                                                                                                    m_layer(0),
                                                                                                                                                    m_layerChanged(false)
    {
      std::vector<unsigned char> hashData(36);
      unsigned int id = m_texID.getID();
      memcpy(&hashData[0], &id, 4);
      memcpy(&hashData[4], &m_animNumber[0], 8);
      memcpy(&hashData[12], &m_animCount[0], 8);
      memcpy(&hashData[20], &m_texStart[0], 8);
      memcpy(&hashData[28], &m_texEnd[0], 8);

      m_hash = MurmurHash64A(&hashData[0], hashData.size(), 0);
    }

    Sprite::Sprite(const Sprite& o)
    {
      m_hash = o.m_hash;

      m_texID = o.m_texID;

      m_rtMatrix = o.m_rtMatrix;
	    m_tlMatrix = o.m_tlMatrix;
	    m_scMatrix = o.m_scMatrix;
	
	    m_animNumber = o.m_animNumber;
	    m_animCount = o.m_animCount;
	    m_texStart = o.m_texStart;
	    m_texEnd = o.m_texEnd;

	    m_BoundingBox[0] = o.m_BoundingBox[0];
      m_BoundingBox[1] = o.m_BoundingBox[1];

      m_angle = o.m_angle;

      m_renderable = o.m_renderable;

      m_layer = o.m_layer;
      m_layerChanged = o.m_layerChanged;
    }

    Sprite& Sprite::operator=(const Sprite& o)
    {
      m_hash = o.m_hash;

      m_texID = o.m_texID;

      m_rtMatrix = o.m_rtMatrix;
	    m_tlMatrix = o.m_tlMatrix;
	    m_scMatrix = o.m_scMatrix;
	
	    m_animNumber = o.m_animNumber;
	    m_animCount = o.m_animCount;
	    m_texStart = o.m_texStart;
	    m_texEnd = o.m_texEnd;

	    m_BoundingBox[0] = o.m_BoundingBox[0];
      m_BoundingBox[1] = o.m_BoundingBox[1];

      m_angle = o.m_angle;

      m_renderable = o.m_renderable;

      m_layer = o.m_layer;
      m_layerChanged = o.m_layerChanged;

      return *this;
    }

    Sprite::~Sprite()
    {
    }

    void Sprite::free()
    {
      m_texID.free();
    }

    void Sprite::setRenderable(bool renderable)
    {
      m_renderable = renderable;
    }

    bool Sprite::getRenderable() const
    {
      return m_renderable;
    }

    void Sprite::setAnimation(unsigned int number)
    {
	    m_animCount = util::Vector<unsigned int, 2>(number % m_animNumber[0], number / m_animNumber[0]);
      assert(m_animCount[1] < m_animNumber[1] && m_animCount[0] < m_animNumber[0]);
    }
    void Sprite::setAnimation(util::Vector<unsigned int, 2> number)
    {
	    m_animCount = number;
      assert(m_animCount[1] < m_animNumber[1] && m_animCount[0] < m_animNumber[0]);
    }

    util::Vector<unsigned int, 2> Sprite::getAnimationNumber()
    {
	    return m_animNumber;
    }

    util::Vector<unsigned int, 2> Sprite::getAnimationCount()
    {
	    return m_animCount;
    }

    void Sprite::setTranslation(util::Vector<float, 2> v)
    {
	    m_tlMatrix[2][0] = v[0];
	    m_tlMatrix[2][1] = v[1];
    }

    void Sprite::setTranslation(float x, float y)
    {
	    m_tlMatrix[2][0] = x;
	    m_tlMatrix[2][1] = y;
    }

    void Sprite::addTranslation(util::Vector<float, 2> v)
    {
	    m_tlMatrix[2][0] += v[0];
	    m_tlMatrix[2][1] += v[1];
    }

    void Sprite::addTranslation(float x, float y)
    {
	    m_tlMatrix[2][0] += x;
	    m_tlMatrix[2][1] += y;
    }

    void Sprite::setScale(float s)
    {
	    m_scMatrix[0][0] = s;
	    m_scMatrix[1][1] = s;
    }

    void Sprite::addScale(float s)
    {
	    m_scMatrix[0][0] += s;
	    m_scMatrix[1][1] += s;
    }

    void Sprite::setScale(util::Vector<float, 2> s)
    {
	    m_scMatrix[0][0] = s[0];
	    m_scMatrix[1][1] = s[1];
    }

    void Sprite::setScale(float sx, float sy)
    {
	    m_scMatrix[0][0] = sx;
	    m_scMatrix[1][1] = sy;
    }

    void Sprite::addScale(util::Vector<float, 2> s)
    {
	    m_scMatrix[0][0] += s[0];
	    m_scMatrix[1][1] += s[1];
    }

    void Sprite::addScale(float sx, float sy)
    {
	    m_scMatrix[0][0] += sx;
	    m_scMatrix[1][1] += sy;
    }

    void Sprite::setRotation(float angle)
    {
	    m_angle = angle;
      m_rtMatrix[0][0] = m_rtMatrix[1][1] = cos(m_angle);
      m_rtMatrix[1][0] = sin(m_angle);
      m_rtMatrix[0][1] = -m_rtMatrix[1][0];
    }

    void Sprite::addRotation(float angle)
    {
	    m_angle += angle;
      m_rtMatrix[0][0] = m_rtMatrix[1][1] = cos(m_angle);
      m_rtMatrix[1][0] = sin(m_angle);
      m_rtMatrix[0][1] = -m_rtMatrix[1][0];
    }

    util::Vector<float, 2> Sprite::getPosition()
    {
      return util::Vector<float, 2>(m_tlMatrix[2][0], m_tlMatrix[2][1]);
    }

    float Sprite::getRotation()
    {
      return m_angle;
    }

    util::Vector<float, 2> Sprite::getScale()
    {
      return util::Vector<float, 2>(m_scMatrix[0][0], m_scMatrix[1][1]);
    }

    void Sprite::setLayer(unsigned char layer)
    {
      m_layer = layer;
      m_layerChanged = true;
    }

    unsigned char Sprite::getLayer()
    {
      return m_layer;
    }

    void Sprite::spriteSorted()
    {
      m_layerChanged = false;
    }


    bool Sprite::getLayerChanged()
    {
      return m_layerChanged;
    }

    util::Matrix<float,3> Sprite::getTransformationMatrix()
    {
	    return m_tlMatrix * m_rtMatrix * m_scMatrix;
    }

    util::Matrix<float,3> Sprite::getTexTransformationMatrix()
    {
	    float width = m_texEnd[0] - m_texStart[0];
	    float height = m_texEnd[1] - m_texStart[1];
	    return util::Matrix<float, 3>(width / m_animNumber[0], 0.0f,                     (static_cast<float>(m_animCount[0]) / static_cast<float>(m_animNumber[0])) * width + m_texStart[0], 
					                    0.0f,                    height / m_animNumber[1], (static_cast<float>(m_animCount[1]) / static_cast<float>(m_animNumber[1])) * height + m_texStart[1], 
					                    0.0f,                    0.0f,                      1.0f);
    }

    util::ResourceHandle Sprite::getTextureID() const
    {
	    return m_texID;
    }

	}
}
