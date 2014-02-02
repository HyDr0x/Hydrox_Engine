#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include <assert.h>

#include "Renderer/DLLExport.h"

#include "Renderer/Resources/ManagedResource.h"
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <Utilities/Math/Math.hpp>

namespace he
{
	namespace renderer
	{
    class GRAPHICAPI Billboard : public ManagedResource
    {
    public:

      Billboard(){}
	    Billboard(util::ResourceHandle texID, bool anim, util::Vector<unsigned int, 2> animNumber, util::Vector<float, 2> texStart, util::Vector<float, 2> texEnd);
      Billboard(const Billboard&);
      Billboard& operator=(const Billboard& o);
	    ~Billboard();

      void free();

      void setRenderable(bool renderable);
	    bool getRenderable() const;

      void setAnimation(unsigned int number);
	    void setAnimation(util::Vector<unsigned int, 2> number);
	    util::Vector<unsigned int, 2> getAnimationNumber();
	    util::Vector<unsigned int, 2> getAnimationCount();

	    void setTranslation(util::Vector<float, 3> v);
	    void setTranslation(float x, float y, float z);
	    void addTranslation(util::Vector<float,3> v);
	    void addTranslation(float x, float y, float z);
	
	    void setScale(float s);
	    void addScale(float s);

	    void setScale(util::Vector<float, 2> s);
	    void setScale(float sx, float sy);
	    void addScale(util::Vector<float, 2> s);
	    void addScale(float sx, float sy);

	    util::Vector<float, 3> getPosition();
	    util::Vector<float, 2> getScale();

	    util::Matrix<float, 3> getTexTransformationMatrix();
	    util::ResourceHandle getTextureID() const;

    private:

	    util::Vector<unsigned int, 2> m_animNumber;
	    util::Vector<unsigned int, 2> m_animCount;
	    util::Vector<float, 2> m_texStart;
	    util::Vector<float, 2> m_texEnd;

	    util::Vector<float, 2> m_scale;
	    util::Vector<float, 3> m_translate;

      util::ResourceHandle m_texID;

      bool m_renderable;
    };
	}
}

#endif
