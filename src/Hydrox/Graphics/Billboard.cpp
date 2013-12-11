#include "Hydrox/Graphics/Billboard.h"

namespace he
{
  Billboard::Billboard(ResourceHandle texID, bool renderable, Vector<unsigned int, 2> animNumber, Vector<float, 2> texStart, Vector<float, 2> texEnd) : m_texID(texID), 
                                                                                                                                                  m_renderable(renderable), 
                                                                                                                                                  m_animNumber(animNumber), 
                                                                                                                                                  m_texStart(texStart), 
                                                                                                                                                  m_texEnd(texEnd),
                                                                                                                                                  m_translate(Vector<float, 3>::identity()),
                                                                                                                                                  m_scale(Vector<float, 2>(1.0f, 1.0f))
  {}

  Billboard::Billboard(const Billboard& o)
  {
    m_texID = o.m_texID;

	  m_animNumber = o.m_animNumber;
	  m_animCount = o.m_animCount;
	  m_texStart = o.m_texStart;
	  m_texEnd = o.m_texEnd;

	  m_scale = o.m_scale;
	  m_translate = o.m_translate;

    m_renderable = o.m_renderable;
  }

  Billboard& Billboard::operator=(const Billboard& o)
  {
    m_texID = o.m_texID;

	  m_animNumber = o.m_animNumber;
	  m_animCount = o.m_animCount;
	  m_texStart = o.m_texStart;
	  m_texEnd = o.m_texEnd;

	  m_scale = o.m_scale;
	  m_translate = o.m_translate;

    m_renderable = o.m_renderable;

    return *this;
  }

  Billboard::~Billboard()
  {}

  void Billboard::free()
  {
    m_texID.free();
  }

  void Billboard::setRenderable(bool renderable)
  {
    m_renderable = renderable;
  }

	bool Billboard::getRenderable() const
  {
    return m_renderable;
  }

  void Billboard::setAnimation(unsigned int number)
  {
    assert(number / m_animNumber[0] < m_animNumber[1]);
    m_animCount = Vector<unsigned int, 2>(number % m_animNumber[0], number / m_animNumber[0]);
  }

  void Billboard::setAnimation(Vector<unsigned int, 2> number)
  {
    assert(number[1] < m_animNumber[1] && number[0] < m_animNumber[0]);
    m_animCount = number;
  }

  Vector<unsigned int, 2> Billboard::getAnimationNumber()
  {
    return m_animNumber;
  }

  Vector<unsigned int, 2> Billboard::getAnimationCount()
  {
    return m_animCount;
  }

  void Billboard::setTranslation(float x, float y, float z)
  {
	  m_translate[0] = x;
	  m_translate[1] = y;
	  m_translate[2] = z;
  }

  void Billboard::setTranslation(Vector<float, 3> v)
  {
	  m_translate = v;
  }

  void Billboard::addTranslation(float x, float y, float z)
  {
	  m_translate[0] += x;
	  m_translate[1] += y;
	  m_translate[2] += z;
  }

  void Billboard::addTranslation(Vector<float, 3> v)
  {
	  m_translate = v;
  }

  void Billboard::setScale(float s)
  {
	  m_scale[0] = s;
	  m_scale[1] = s;
  }

  void Billboard::addScale(float s)
  {
	  m_scale[0] += s;
	  m_scale[1] += s;
  }

  void Billboard::setScale(Vector<float, 2> s)
  {
	  m_scale = s;
  }

  void Billboard::setScale(float sx, float sy)
  {
	  m_scale[0] = sx;
	  m_scale[1] = sy;
  }

  void Billboard::addScale(Vector<float, 2> s)
  {
	  m_scale = s;
  }

  void Billboard::addScale(float sx, float sy)
  {
	  m_scale[0] += sx;
	  m_scale[1] += sy;
  }

  Vector<float, 3> Billboard::getPosition()
  {
	  return m_translate;
  }

  Vector<float, 2> Billboard::getScale()
  {
	  return m_scale;
  }

  Matrix<float, 3> Billboard::getTexTransformationMatrix()
  {
	  float width  = m_texEnd[0] - m_texStart[0];
	  float height = m_texEnd[1] - m_texStart[1];

    return Matrix<float,3>(width/m_animNumber[0],0.0f,(static_cast<float>(m_animCount[0])/static_cast<float>(m_animNumber[0]))*width+m_texStart[0], 
					    0.0f,height/m_animNumber[1],(static_cast<float>(m_animCount[1])/static_cast<float>(m_animNumber[1]))*height+m_texStart[1], 
					    0.0f,0.0f,1.0f);
  }

  ResourceHandle Billboard::getTextureID() const
  {
	  return m_texID;
  }
}