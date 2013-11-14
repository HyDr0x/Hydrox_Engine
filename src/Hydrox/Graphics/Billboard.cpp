#include "Hydrox/Graphics/Billboard.h"


Billboard::Billboard(ResourceHandle texID, bool anim, Vec<unsigned int,2> animNumber, Vec<float,2> texStart, Vec<float,2> texEnd)  : m_texID(texID), 
                                                                                                                                     m_anim(anim), 
                                                                                                                                     m_animNumber(animNumber), 
                                                                                                                                     m_texStart(texStart), 
                                                                                                                                     m_texEnd(texEnd),
                                                                                                                                     m_translate(Vec<float, 3>::identity()),
                                                                                                                                     m_scale(Vec<float, 2>(1.0f, 1.0f))
{}

Billboard& Billboard::operator=(const Billboard& o)
{
  m_texID = o.m_texID;

	m_animNumber = o.m_animNumber;
	m_animCount = o.m_animCount;
	m_texStart = o.m_texStart;
	m_texEnd = o.m_texEnd;

	m_scale = o.m_scale;
	m_translate = o.m_translate;

  m_anim = o.m_anim;

  return *this;
}

Billboard::~Billboard()
{}

void Billboard::free()
{
}

void Billboard::setAnimation(unsigned int number)
{
  assert(number / m_animNumber[0] < m_animNumber[1]);
  m_animCount = Vec<unsigned int,2>(number % m_animNumber[0], number / m_animNumber[0]);
}

void Billboard::setAnimation(Vec<unsigned int,2> number)
{
  assert(number[1] < m_animNumber[1] && number[0] < m_animNumber[0]);
  m_animCount = number;
}

Vec<unsigned int,2> Billboard::getAnimationNumber()
{
  return m_animNumber;
}

Vec<unsigned int,2> Billboard::getAnimationCount()
{
  return m_animCount;
}

void Billboard::setPosition(float x, float y, float z)
{
	m_translate[0] = x;
	m_translate[1] = y;
	m_translate[2] = z;
}

void Billboard::setPosition(Vec<float,3> v)
{
	m_translate = v;
}

void Billboard::setTranslation(float x, float y, float z)
{
	m_translate[0] += x;
	m_translate[1] += y;
	m_translate[2] += z;
}

void Billboard::setTranslation(Vec<float,3> v)
{
	m_translate = v;
}


void Billboard::setScaling(Vec<float,2> s)
{
	m_scale = s;
}

void Billboard::setScaling(float sx, float sy)
{
	m_scale[0] = sx;
	m_scale[1] = sy;
}

void Billboard::scale(Vec<float,2> s)
{
	m_scale = s;
}

void Billboard::scale(float sx, float sy)
{
	m_scale[0] += sx;
	m_scale[1] += sy;
}

void Billboard::setScaling(float s)
{
	m_scale[0] = s;
	m_scale[1] = s;
}

void Billboard::scale(float s)
{
	m_scale[0] += s;
	m_scale[1] += s;
}


Vec<float,3> Billboard::getPosition()
{
	return m_translate;
}

Vec<float,2> Billboard::getScale()
{
	return m_scale;
}

Mat<float,3> Billboard::getTexTransformationMatrix()
{
	float width  = m_texEnd[0] - m_texStart[0];
	float height = m_texEnd[1] - m_texStart[1];

  return Mat<float,3>(width/m_animNumber[0],0.0f,(static_cast<float>(m_animCount[0])/static_cast<float>(m_animNumber[0]))*width+m_texStart[0], 
					  0.0f,height/m_animNumber[1],(static_cast<float>(m_animCount[1])/static_cast<float>(m_animNumber[1]))*height+m_texStart[1], 
					  0.0f,0.0f,1.0f);
}

ResourceHandle Billboard::getTextureID() const
{
	return m_texID;
}
