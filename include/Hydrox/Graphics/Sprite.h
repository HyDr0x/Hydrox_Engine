#ifndef SPRITE_H_
#define SPRITE_H_

#include <assert.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/CachedResource.h"

#include "Hydrox/Utility/Math/Math.hpp"

class GRAPHICAPI Sprite : public CachedResource
{
public:

  Sprite(){}
	Sprite(const Sprite&){}

	Sprite(ResourceHandle texID, bool anim, Vector<unsigned int, 2> animNumber, Vector<float, 2> texStart, Vector<float, 2> texEnd);
  Sprite& operator=(const Sprite& o);
	~Sprite();

  void free();

	void setAnimation(unsigned int number);
	void setAnimation(Vector<unsigned int, 2> number);
	Vector<unsigned int, 2> getAnimationNumber();
	Vector<unsigned int, 2> getAnimationCount();

	void setPosition(Vector<float, 2> v);

	void setPosition(float x, float y);//set Position
	void setTranslation(Vector<float, 2> v);
	void setTranslation(float x, float y);

	void setScaling(float s);
	void scale(float s);

	void setScaling(Vector<float, 2> s);
	void setScaling(float sx, float sy);
	void scale(Vector<float, 2> s);
	void scale(float sx, float sy);

	void setRotation(float angle);

	Matrix<float, 3> getTransformationMatrix();
	Matrix<float, 3> getTexTransformationMatrix();
	ResourceHandle getTextureID() const;

private:

  ResourceHandle m_texID;

  Matrix<float, 3> m_rtMatrix;
	Matrix<float, 3> m_tlMatrix;
	Matrix<float, 3> m_scMatrix;
	
	Vector<unsigned int, 2> m_animNumber;
	Vector<unsigned int, 2> m_animCount;
	Vector<float, 2> m_texStart;
	Vector<float, 2> m_texEnd;

	Vector<float, 2> m_BoundingBox[2];

  float m_angle;

  bool m_anim;
};

#endif