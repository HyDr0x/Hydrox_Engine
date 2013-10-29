#include "Hydrox/Graphics/Sprite.h"


Sprite::Sprite(ResourceHandle m_texID, bool m_anim, Vec<unsigned int, 2> m_animNumber, Vec<float, 2> m_texStart, Vec<float, 2> m_texEnd) :  m_texID(m_texID), 
                                                                                                                                                            m_anim(m_anim), 
                                                                                                                                                            m_animNumber(m_animNumber), 
                                                                                                                                                            m_texStart(m_texStart),
                                                                                                                                                            m_texEnd(m_texEnd),
                                                                                                                                                            m_rtMatrix(Mat<float, 3>::identity()),
                                                                                                                                                            m_tlMatrix(Mat<float, 3>::identity()),
                                                                                                                                                            m_scMatrix(Mat<float, 3>::identity())
{
}

Sprite& Sprite::operator=(const Sprite& o)
{
  m_texID = o.m_texID;

  m_rtMatrix = o.m_rtMatrix;
	m_tlMatrix = o.m_tlMatrix;
	m_scMatrix = o.m_scMatrix;
	
	m_animNumber = o.m_animNumber;
	m_animCount = o.m_animCount;
	m_texStart = o.m_texStart;
	m_texEnd = o.m_texEnd;

	m_BoundingBox[0] = o.m_BoundingBox[0];
  m_BoundingBox[1] = o.m_BoundingBox[1];

  m_angle = o.m_angle;

  m_anim = o.m_anim;

  return *this;
}

Sprite::~Sprite()
{
}

void Sprite::free()
{
}

void Sprite::setAnimation(unsigned int number)
{
  assert(m_animCount[1] < m_animNumber[1] && m_animCount[0] < m_animNumber[0]);
	m_animCount = Vec<unsigned int, 2>(number % m_animNumber[0], number / m_animNumber[0]);
}
void Sprite::setAnimation(Vec<unsigned int,2> number)
{
  assert(m_animCount[1] < m_animNumber[1] && m_animCount[0] < m_animNumber[0]);
	m_animCount = number;
}

Vec<unsigned int, 2> Sprite::getAnimationNumber()
{
	return m_animNumber;
}

Vec<unsigned int, 2> Sprite::getAnimationCount()
{
	return m_animCount;
}

void Sprite::setPosition(Vec<float, 2> v)
{
	m_tlMatrix[2][0] = v[0];
	m_tlMatrix[2][1] = v[1];
}

void Sprite::setPosition(float x, float y)
{
	m_tlMatrix[2][0] = x;
	m_tlMatrix[2][1] = y;
}

void Sprite::setTranslation(Vec<float, 2> v)
{
	m_tlMatrix[2][0] += v[0];
	m_tlMatrix[2][1] += v[1];
}

void Sprite::setTranslation(float x, float y)
{
	m_tlMatrix[2][0] += x;
	m_tlMatrix[2][1] += y;
}

void Sprite::setScaling(float s)
{
	m_scMatrix[0][0] = s;
	m_scMatrix[1][1] = s;
}

void Sprite::scale(float s)
{
	m_scMatrix[0][0] += s;
	m_scMatrix[1][1] += s;
}

void Sprite::setScaling(Vec<float, 2> s)
{
	m_scMatrix[0][0] = s[0];
	m_scMatrix[1][1] = s[1];
}

void Sprite::setScaling(float sx, float sy)
{
	m_scMatrix[0][0] = sx;
	m_scMatrix[1][1] = sy;
}

void Sprite::scale(Vec<float, 2> s)
{
	m_scMatrix[0][0] += s[0];
	m_scMatrix[1][1] += s[1];
}

void Sprite::scale(float sx, float sy)
{
	m_scMatrix[0][0] += sx;
	m_scMatrix[1][1] += sy;
}

void Sprite::setRotation(float m_angle)
{
	this->m_angle= m_angle;
    m_rtMatrix[0][0] = m_rtMatrix[1][1] = cos(this->m_angle);
    m_rtMatrix[1][0] = sin(this->m_angle);
    m_rtMatrix[0][1] = -m_rtMatrix[1][0];
}

Mat<float,3> Sprite::getTransformationMatrix()
{
	return m_tlMatrix * m_rtMatrix * m_scMatrix;
}

Mat<float,3> Sprite::getTexTransformationMatrix()
{
	float width = m_texEnd[0] - m_texStart[0];
	float height = m_texEnd[1] - m_texStart[1];
	return Mat<float,3>(width/m_animNumber[0],0.0f,(static_cast<float>(m_animCount[0])/static_cast<float>(m_animNumber[0]))*width+m_texStart[0], 
					  0.0f,height/m_animNumber[1],(static_cast<float>(m_animCount[1])/static_cast<float>(m_animNumber[1]))*height+m_texStart[1], 
					  0.0f,0.0f,1.0f);
}

ResourceHandle Sprite::getTextureID() const
{
	return m_texID;
}
