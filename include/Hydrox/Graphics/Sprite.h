#ifndef SPRITE_H_
#define SPRITE_H_

#include <assert.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/CachedResource.h"

#include "Hydrox/Utility/Math/Math.hpp"

namespace he
{
  class GRAPHICAPI Sprite : public CachedResource
  {
  public:

    Sprite(){}
	  Sprite(ResourceHandle texID, bool anim, Vector<unsigned int, 2> animNumber, Vector<float, 2> texStart, Vector<float, 2> texEnd);
    Sprite(const Sprite& o);
    Sprite& operator=(const Sprite& o);
	  ~Sprite();

    void free();

    void setRenderable(bool renderable);
	  bool getRenderable() const;

	  void setAnimation(unsigned int number);
	  void setAnimation(Vector<unsigned int, 2> number);
	  Vector<unsigned int, 2> getAnimationNumber();
	  Vector<unsigned int, 2> getAnimationCount();

	  void setTranslation(Vector<float, 2> v);
	  void setTranslation(float x, float y);

	  void addTranslation(Vector<float, 2> v);
	  void addTranslation(float x, float y);

	  void setScale(float s);
	  void addScale(float s);

	  void setScale(Vector<float, 2> s);
	  void setScale(float sx, float sy);
	  void addScale(Vector<float, 2> s);
	  void addScale(float sx, float sy);

	  void setRotation(float angle);
    void addRotation(float angle);

    Vector<float, 2> getPosition();
    float getRotation();
    Vector<float, 2> getScale();

    void setZValue(float z);
    float getZValue();

	  Matrix<float, 3> getTransformationMatrix();
	  Matrix<float, 3> getTexTransformationMatrix();
	  ResourceHandle getTextureID() const;

  private:

    ResourceHandle m_texID;

    Matrix<float, 3> m_rtMatrix;
	  Matrix<float, 3> m_tlMatrix;
	  Matrix<float, 3> m_scMatrix;
    float m_z;
	
	  Vector<unsigned int, 2> m_animNumber;
	  Vector<unsigned int, 2> m_animCount;
	  Vector<float, 2> m_texStart;
	  Vector<float, 2> m_texEnd;

	  Vector<float, 2> m_BoundingBox[2];

    float m_angle;

    bool m_renderable;//boolean which decides if the sprite is being drawn or not
  };
}

#endif