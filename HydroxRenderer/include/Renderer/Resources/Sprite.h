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

	    Sprite(util::EventManager *eventManager, util::ResourceHandle textureHandle, bool renderable, bool transparency, const util::Vector<unsigned int, 2>& animNumber, const util::Vector<float, 2>& texStart, const util::Vector<float, 2>& texEnd);
      Sprite(const Sprite& o);
	    ~Sprite();

      Sprite& operator=(const Sprite& o);

      void setRenderable(bool renderable);
	    bool getRenderable() const;

      void setTransparency(bool transparency);
	    bool getTransparency() const;

	    void setAnimation(unsigned int number);
	    void setAnimation(const util::Vector<unsigned int, 2>& number);
	    util::Vector<unsigned int, 2> getAnimationNumber() const;
	    util::Vector<unsigned int, 2> getAnimationCount() const;

	    void setTranslation(const util::Vector<float, 2>& v);
	    void setTranslation(float x, float y);

	    void addTranslation(const util::Vector<float, 2>& v);
	    void addTranslation(float x, float y);

	    void setScale(float s);
	    void addScale(float s);

	    void setScale(const util::Vector<float, 2>& s);
	    void setScale(float sx, float sy);
	    void addScale(const util::Vector<float, 2>& s);
	    void addScale(float sx, float sy);

	    void setRotation(float angle);
      void addRotation(float angle);

      util::Vector<float, 2> getPosition() const;
      float getRotation() const;
      util::Vector<float, 2> getScale() const;

      void setLayer(unsigned char layer);
      unsigned char getLayer() const;

	    util::Matrix<float, 3> getTransformationMatrix() const;
	    util::Matrix<float, 3> getTexTransformationMatrix() const;
	    util::ResourceHandle getTextureHandle() const;

    private:

      Sprite();

      util::EventManager *m_eventManager;

      util::ResourceHandle m_textureHandle;

      util::Matrix<float, 3> m_rtMatrix;
	    util::Matrix<float, 3> m_tlMatrix;
	    util::Matrix<float, 3> m_scMatrix;
      unsigned char m_layer;

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
