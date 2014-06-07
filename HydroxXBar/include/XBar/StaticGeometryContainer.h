#ifndef STATICGEOMETRYCONTAINER_H_
#define STATICGEOMETRYCONTAINER_H_

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "XBar/DLLExport.h"

#include "XBar/IGeometryContainer.h"

namespace he
{
  namespace xBar
  {
    class GRAPHICAPI StaticGeometryContainer : public IGeometryContainer
    {
    public:

      StaticGeometryContainer(util::Matrix<float, 4> *trfMatrix, util::ResourceHandle materialHandle, util::ResourceHandle meshHandle);
      virtual ~StaticGeometryContainer();

    protected:

      void createHash();
    };
  }
}

#endif