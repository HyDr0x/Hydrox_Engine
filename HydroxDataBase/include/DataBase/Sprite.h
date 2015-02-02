#ifndef SPRITE_H_
#define SPRITE_H_

#include <cassert>

#include "DataBase/DLLExport.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>
#include <Utilities/Pointer/SharedPointer.h>
#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace db
  {
    class GRAPHICAPI Sprite
    {
    public:

      Sprite(util::SharedPointer<util::EventManager> eventManager, util::ResourceHandle textureHandle, bool renderable, bool transparency, const util::vec2ui& animNumber, const util::vec2f& texStart, const util::vec2f& texEnd);
      Sprite(const Sprite& other);
      ~Sprite();

      Sprite& operator=(const Sprite& other);

      void setRenderable(bool renderable);
      bool getRenderable() const;

      void setTransparency(bool transparency);
      bool getTransparency() const;

      void setAnimation(unsigned int number);
      void setAnimation(const util::vec2ui& number);
      util::vec2ui getAnimationNumber() const;
      util::vec2ui getAnimationCount() const;

      void setTranslation(const util::vec2f& v);
      void setTranslation(float x, float y);

      void addTranslation(const util::vec2f& v);
      void addTranslation(float x, float y);

      void setScale(float s);
      void addScale(float s);

      void setScale(const util::vec2f& s);
      void setScale(float sx, float sy);
      void addScale(const util::vec2f& s);
      void addScale(float sx, float sy);

      void setRotation(float angle);
      void addRotation(float angle);

      util::vec2f getPosition() const;
      float getRotation() const;
      util::vec2f getScale() const;

      void setLayer(unsigned char layer);
      unsigned char getLayer() const;

      util::Matrix<float, 3> getTransformationMatrix() const;
      util::Matrix<float, 3> getTexTransformationMatrix() const;
      util::ResourceHandle getTextureHandle() const;

    private:

      Sprite();

      util::SharedPointer<util::EventManager> m_eventManager;

      util::ResourceHandle m_textureHandle;

      util::Matrix<float, 3> m_rtMatrix;
      util::Matrix<float, 3> m_tlMatrix;
      util::Matrix<float, 3> m_scMatrix;
      unsigned char m_layer;

      util::vec2ui m_animNumber;
      util::vec2ui m_animCount;
      util::vec2f m_texStart;
      util::vec2f m_texEnd;

      util::vec2f m_BoundingBox[2];

      float m_angle;

      bool m_transparency;
      bool m_renderable;//boolean which decides if the sprite is being drawn or not
    };
  }
}

#endif
