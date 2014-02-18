#ifndef SPRITE_H_
#define SPRITE_H_

#include <assert.h>

#include "Renderer/DLLExport.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>

#include <Utilities/Math/Math.hpp>

namespace he
{
	namespace renderer
	{
    class GRAPHICAPI Sprite
    {
    public:

	    Sprite(util::EventManager *eventManager, util::ResourceHandle textureHandle, bool renderable, bool transparency, util::Vector<unsigned int, 2> animNumber, util::Vector<float, 2> texStart, util::Vector<float, 2> texEnd);
      Sprite(const Sprite& o);
      Sprite& operator=(const Sprite& o);
	    ~Sprite();

      void setRenderable(bool renderable);
	    bool getRenderable() const;

      void setTransparency(bool transparency);
	    bool getTransparency() const;

	    void setAnimation(unsigned int number);
	    void setAnimation(util::Vector<unsigned int, 2> number);
	    util::Vector<unsigned int, 2> getAnimationNumber();
	    util::Vector<unsigned int, 2> getAnimationCount();

	    void setTranslation(util::Vector<float, 2> v);
	    void setTranslation(float x, float y);

	    void addTranslation(util::Vector<float, 2> v);
	    void addTranslation(float x, float y);

	    void setScale(float s);
	    void addScale(float s);

	    void setScale(util::Vector<float, 2> s);
	    void setScale(float sx, float sy);
	    void addScale(util::Vector<float, 2> s);
	    void addScale(float sx, float sy);

	    void setRotation(float angle);
      void addRotation(float angle);

      util::Vector<float, 2> getPosition();
      float getRotation();
      util::Vector<float, 2> getScale();

      void setLayer(unsigned char layer);
      unsigned char getLayer();

      void spriteSorted();
      bool getLayerChanged();

	    util::Matrix<float, 3> getTransformationMatrix();
	    util::Matrix<float, 3> getTexTransformationMatrix();
	    util::ResourceHandle getTextureHandle() const;

    private:

      Sprite(){}

      util::EventManager *m_eventManager;

      util::ResourceHandle m_textureHandle;

      util::Matrix<float, 3> m_rtMatrix;
	    util::Matrix<float, 3> m_tlMatrix;
	    util::Matrix<float, 3> m_scMatrix;
      unsigned char m_layer;
	    bool m_layerChanged;

	    util::Vector<unsigned int, 2> m_animNumber;
	    util::Vector<unsigned int, 2> m_animCount;
	    util::Vector<float, 2> m_texStart;
	    util::Vector<float, 2> m_texEnd;

	    util::Vector<float, 2> m_BoundingBox[2];

      float m_angle;

      bool m_transparency;
      bool m_renderable;//boolean which decides if the sprite is being drawn or not
    };
	}
}

#endif
