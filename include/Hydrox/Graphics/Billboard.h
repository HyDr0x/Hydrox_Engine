#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include <assert.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Miscellaneous/CachedResource.h"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

#include "Hydrox/Utility/Math/Math.hpp"

namespace he
{
  class GRAPHICAPI Billboard : public CachedResource
  {
  public:

    Billboard(){}
	  Billboard(ResourceHandle texID, bool anim, Vector<unsigned int, 2> animNumber, Vector<float, 2> texStart, Vector<float, 2> texEnd);
    Billboard(const Billboard&);
    Billboard& operator=(const Billboard& o);
	  ~Billboard();

    void free();

    void setRenderable(bool renderable);
	  bool getRenderable() const;

    void setAnimation(unsigned int number);
	  void setAnimation(Vector<unsigned int, 2> number);
	  Vector<unsigned int, 2> getAnimationNumber();
	  Vector<unsigned int, 2> getAnimationCount();

	  void setTranslation(Vector<float, 3> v);
	  void setTranslation(float x, float y, float z);
	  void addTranslation(Vector<float,3> v);
	  void addTranslation(float x, float y, float z);
	
	  void setScale(float s);
	  void addScale(float s);

	  void setScale(Vector<float, 2> s);
	  void setScale(float sx, float sy);
	  void addScale(Vector<float, 2> s);
	  void addScale(float sx, float sy);

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

    bool m_renderable;
  };
}

#endif
