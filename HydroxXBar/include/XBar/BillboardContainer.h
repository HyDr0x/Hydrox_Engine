#ifndef BILLBOARDCONTAINER_H_
#define BILLBOARDCONTAINER_H_

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "XBar/DLLExport.h"

namespace he
{
	namespace xBar
	{
    class GRAPHICAPI BillboardContainer
    {
    public:

      BillboardContainer(util::Vector<float, 3>& translation, 
                        util::Vector<float, 2>& scale, 
                        util::Vector<unsigned int, 2>& animNumber, 
                        util::Vector<unsigned int, 2>& animCount, 
                        util::Vector<float, 2>& texStart, 
                        util::Vector<float, 2>& texEnd, 
                        util::ResourceHandle textureHandle);

      ~BillboardContainer();

      BillboardContainer* clone();

      bool operator ==(const BillboardContainer& o);

      util::ResourceHandle getTextureHandle() const;

      util::Vector<float, 3> getPosition();
	    util::Vector<float, 2> getScale();

      util::Matrix<float, 3> getTexTransformationMatrix();

    private:

      util::Vector<unsigned int, 2>& m_animNumber;
	    util::Vector<unsigned int, 2>& m_animCount;
	    util::Vector<float, 2>& m_texStart;
	    util::Vector<float, 2>& m_texEnd;

	    util::Vector<float, 2>& m_scale;
	    util::Vector<float, 3>& m_translate;

      util::ResourceHandle m_textureHandle;
    };
  }
}

#endif