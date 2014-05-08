#ifndef STATICGEOMETRYCONTAINER_H_
#define STATICGEOMETRYCONTAINER_H_

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "XBar/DLLExport.h"

#include "XBar/AContainer.h"

namespace he
{
	namespace xBar
	{
    class GRAPHICAPI StaticGeometryContainer : public AContainer
    {
    public:

      StaticGeometryContainer(util::Matrix<float, 4> *trfMatrix, util::ResourceHandle materialHandle, util::ResourceHandle meshHandle);
      virtual ~StaticGeometryContainer();

      bool operator == (const StaticGeometryContainer& o);

      util::ResourceHandle getMaterialHandle();

      util::ResourceHandle getMeshHandle();

      util::Matrix<float, 4> getTransformationMatrix();

    protected:

      virtual void createHash();

      util::Matrix<float, 4> *m_trfMatrix;

      util::ResourceHandle m_materialHandle;
      util::ResourceHandle m_meshHandle;
    };
  }
}

#endif