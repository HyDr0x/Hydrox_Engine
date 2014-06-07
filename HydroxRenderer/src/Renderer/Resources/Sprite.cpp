#include "Renderer/Resources/Sprite.h"

#include <vector>

namespace he
{
  namespace renderer
  {
    Sprite::Sprite(util::EventManager *eventManager, util::ResourceHandle textureHandle, bool renderable, bool transparency, const util::Vector<unsigned int, 2>& animNumber, const util::Vector<float, 2>& texStart, const util::Vector<float, 2>& texEnd) : 
      m_eventManager(eventManager),
      m_textureHandle(textureHandle), 
      m_renderable(renderable), 
      m_transparency(transparency),
      m_animNumber(animNumber), 
      m_animCount(util::Vector<unsigned int, 2>(0, 0)),
      m_texStart(texStart),
      m_texEnd(texEnd),
      m_angle(0.0f),
      m_rtMatrix(util::Matrix<float, 3>::identity()),
      m_tlMatrix(util::Matrix<float, 3>::identity()),
      m_scMatrix(util::Matrix<float, 3>::identity()),
      m_layer(0)
    {
    }

    Sprite::Sprite(const Sprite& other)
    {
      m_eventManager = other.m_eventManager;

      m_textureHandle = other.m_textureHandle;

      m_rtMatrix = other.m_rtMatrix;
      m_tlMatrix = other.m_tlMatrix;
      m_scMatrix = other.m_scMatrix;
  
      m_animNumber = other.m_animNumber;
      m_animCount = other.m_animCount;
      m_texStart = other.m_texStart;
      m_texEnd = other.m_texEnd;

      m_BoundingBox[0] = other.m_BoundingBox[0];
      m_BoundingBox[1] = other.m_BoundingBox[1];

      m_angle = other.m_angle;

      m_renderable = other.m_renderable;
      m_transparency = other.m_transparency;

      m_layer = other.m_layer;
    }

    Sprite::~Sprite()
    {
    }

    Sprite& Sprite::operator=(const Sprite& other)
    {
      m_eventManager = other.m_eventManager;

      m_textureHandle = other.m_textureHandle;

      m_rtMatrix = other.m_rtMatrix;
      m_tlMatrix = other.m_tlMatrix;
      m_scMatrix = other.m_scMatrix;
  
      m_animNumber = other.m_animNumber;
      m_animCount = other.m_animCount;
      m_texStart = other.m_texStart;
      m_texEnd = other.m_texEnd;

      m_BoundingBox[0] = other.m_BoundingBox[0];
      m_BoundingBox[1] = other.m_BoundingBox[1];

      m_angle = other.m_angle;

      m_renderable = other.m_renderable;
      m_transparency = other.m_transparency;

      m_layer = other.m_layer;

      return *this;
    }

    void Sprite::setRenderable(bool renderable)
    {
      if(!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void (*)(const Sprite *sprite)>(util::EventManager::OnAddSprite)->execute(this);
      }
      else if(m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void (*)(const Sprite *sprite)>(util::EventManager::OnRemoveSprite)->execute(this);
      }

      m_renderable = renderable;
    }

    bool Sprite::getRenderable() const
    {
      return m_renderable;
    }

    void Sprite::setTransparency(bool transparency)
    {
      if (m_renderable)
      {
        if (m_transparency != transparency)
        {
          m_eventManager->raiseSignal<void(*)(const Sprite *sprite)>(util::EventManager::OnRemoveSprite)->execute(this);

          m_transparency = transparency;

          m_eventManager->raiseSignal<void(*)(const Sprite *sprite)>(util::EventManager::OnAddSprite)->execute(this);
        }
      }
      else
      {
        m_transparency = transparency;
      }
    }

    bool Sprite::getTransparency() const
    {
      return m_transparency;
    }

    void Sprite::setAnimation(unsigned int number)
    {
      m_animCount = util::Vector<unsigned int, 2>(number % m_animNumber[0], number / m_animNumber[0]);
      assert(m_animCount[1] < m_animNumber[1] && m_animCount[0] < m_animNumber[0]);
    }
    void Sprite::setAnimation(const util::Vector<unsigned int, 2>& number)
    {
      m_animCount = number;
      assert(m_animCount[1] < m_animNumber[1] && m_animCount[0] < m_animNumber[0]);
    }

    util::Vector<unsigned int, 2> Sprite::getAnimationNumber() const
    {
      return m_animNumber;
    }

    util::Vector<unsigned int, 2> Sprite::getAnimationCount() const
    {
      return m_animCount;
    }

    void Sprite::setTranslation(const util::Vector<float, 2>& v)
    {
      m_tlMatrix[2][0] = v[0];
      m_tlMatrix[2][1] = v[1];
    }

    void Sprite::setTranslation(float x, float y)
    {
      m_tlMatrix[2][0] = x;
      m_tlMatrix[2][1] = y;
    }

    void Sprite::addTranslation(const util::Vector<float, 2>& v)
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

    void Sprite::setScale(const util::Vector<float, 2>& s)
    {
      m_scMatrix[0][0] = s[0];
      m_scMatrix[1][1] = s[1];
    }

    void Sprite::setScale(float sx, float sy)
    {
      m_scMatrix[0][0] = sx;
      m_scMatrix[1][1] = sy;
    }

    void Sprite::addScale(const util::Vector<float, 2>& s)
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

    util::Vector<float, 2> Sprite::getPosition() const
    {
      return util::Vector<float, 2>(m_tlMatrix[2][0], m_tlMatrix[2][1]);
    }

    float Sprite::getRotation() const
    {
      return m_angle;
    }

    util::Vector<float, 2> Sprite::getScale() const
    {
      return util::Vector<float, 2>(m_scMatrix[0][0], m_scMatrix[1][1]);
    }

    void Sprite::setLayer(unsigned char layer)
    {
      if(m_renderable && m_transparency)
      {
        if(m_layer != layer)
        {
          m_eventManager->raiseSignal<void(*)(const Sprite *sprite)>(util::EventManager::OnRemoveSprite)->execute(this);

          m_layer = layer;

          m_eventManager->raiseSignal<void(*)(const Sprite *sprite)>(util::EventManager::OnAddSprite)->execute(this);
        }
      }
      else
      {
        m_layer = layer;
      }
    }

    unsigned char Sprite::getLayer() const
    {
      return m_layer;
    }

    util::Matrix<float,3> Sprite::getTransformationMatrix() const
    {
      return m_tlMatrix * m_rtMatrix * m_scMatrix;
    }

    util::Matrix<float,3> Sprite::getTexTransformationMatrix() const
    {
      float width  = m_texEnd[0] - m_texStart[0];
      float height = m_texEnd[1] - m_texStart[1];
      return util::Matrix<float, 3>(width / m_animNumber[0], 0.0f,                     (static_cast<float>(m_animCount[0]) / static_cast<float>(m_animNumber[0])) * width  + m_texStart[0], 
                                    0.0f,                    height / m_animNumber[1], (static_cast<float>(m_animCount[1]) / static_cast<float>(m_animNumber[1])) * height + m_texStart[1], 
                                    0.0f,                    0.0f,                      1.0f);
    }

    util::ResourceHandle Sprite::getTextureHandle() const
    {
      return m_textureHandle;
    }
  }
}
