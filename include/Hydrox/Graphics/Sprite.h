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

	Sprite(ResourceHandle texID, bool anim, Vec<unsigned int, 2> animNumber, Vec<float, 2> texStart, Vec<float, 2> texEnd);
  Sprite& operator=(const Sprite& o);
	~Sprite();

  void free();

	void setAnimation(unsigned int number);
	void setAnimation(Vec<unsigned int, 2> number);
	Vec<unsigned int, 2> getAnimationNumber();
	Vec<unsigned int, 2> getAnimationCount();

	void setPosition(Vec<float, 2> v);

	void setPosition(float x, float y);//set Position
	void setTranslation(Vec<float, 2> v);
	void setTranslation(float x, float y);

	void setScaling(float s);
	void scale(float s);

	void setScaling(Vec<float, 2> s);
	void setScaling(float sx, float sy);
	void scale(Vec<float, 2> s);
	void scale(float sx, float sy);

	void setRotation(float angle);

	Mat<float, 3> getTransformationMatrix();
	Mat<float, 3> getTexTransformationMatrix();
	ResourceHandle getTextureID() const;

private:

  ResourceHandle m_texID;

  Mat<float, 3> m_rtMatrix;
	Mat<float, 3> m_tlMatrix;
	Mat<float, 3> m_scMatrix;
	
	Vec<unsigned int, 2> m_animNumber;
	Vec<unsigned int, 2> m_animCount;
	Vec<float, 2> m_texStart;
	Vec<float, 2> m_texEnd;

	Vec<float, 2> m_BoundingBox[2];

  float m_angle;

  bool m_anim;
};

#endif