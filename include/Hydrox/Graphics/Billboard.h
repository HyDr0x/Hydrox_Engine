#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include <assert.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/CachedResource.h"

#include "Hydrox/Utility/Math/Math.hpp"

class GRAPHICAPI Billboard : public CachedResource
{
public:

  Billboard(){}
	Billboard(const Billboard&){}

	Billboard(ResourceHandle texID, bool anim, Vec<unsigned int, 2> animNumber, Vec<float, 2> texStart, Vec<float, 2> texEnd);
  Billboard& operator=(const Billboard& o);
	~Billboard();

  void free();

  void setAnimation(unsigned int number);
	void setAnimation(Vec<unsigned int, 2> number);
	Vec<unsigned int, 2> getAnimationNumber();
	Vec<unsigned int, 2> getAnimationCount();

	void setPosition(Vec<float, 3> v);
	void setPosition(float x, float y, float z);
	void setTranslation(Vec<float,3> v);
	void setTranslation(float x, float y, float z);
	
	void setScaling(float s);
	void scale(float s);

	void setScaling(Vec<float, 2> s);
	void setScaling(float sx, float sy);
	void scale(Vec<float, 2> s);
	void scale(float sx, float sy);

	Vec<float, 3> getPosition();
	Vec<float, 2> getScale();

	Mat<float, 3> getTexTransformationMatrix();
	ResourceHandle getTextureID() const;

private:

	Vec<unsigned int, 2> m_animNumber;
	Vec<unsigned int, 2> m_animCount;
	Vec<float, 2> m_texStart;
	Vec<float, 2> m_texEnd;

	Vec<float, 2> m_scale;
	Vec<float, 3> m_translate;

  ResourceHandle m_texID;

  bool m_anim;
};

#endif