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

	Billboard(ResourceHandle texID, bool anim, Vector<unsigned int, 2> animNumber, Vector<float, 2> texStart, Vector<float, 2> texEnd);
  Billboard& operator=(const Billboard& o);
	~Billboard();

  void free();

  void setAnimation(unsigned int number);
	void setAnimation(Vector<unsigned int, 2> number);
	Vector<unsigned int, 2> getAnimationNumber();
	Vector<unsigned int, 2> getAnimationCount();

	void setPosition(Vector<float, 3> v);
	void setPosition(float x, float y, float z);
	void setTranslation(Vector<float,3> v);
	void setTranslation(float x, float y, float z);
	
	void setScaling(float s);
	void scale(float s);

	void setScaling(Vector<float, 2> s);
	void setScaling(float sx, float sy);
	void scale(Vector<float, 2> s);
	void scale(float sx, float sy);

	Vector<float, 3> getPosition();
	Vector<float, 2> getScale();

	Matrix<float, 3> getTexTransformationMatrix();
	ResourceHandle getTextureID() const;

private:

	Vector<unsigned int, 2> m_animNumber;
	Vector<unsigned int, 2> m_animCount;
	Vector<float, 2> m_texStart;
	Vector<float, 2> m_texEnd;

	Vector<float, 2> m_scale;
	Vector<float, 3> m_translate;

  ResourceHandle m_texID;

  bool m_anim;
};

#endif