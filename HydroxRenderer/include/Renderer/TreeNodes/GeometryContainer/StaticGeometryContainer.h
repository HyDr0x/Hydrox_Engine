#ifndef STATICGEOMETRYCONTAINER_H_
#define STATICGEOMETRYCONTAINER_H_

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

namespace he
{
	namespace renderer
	{
    class StaticGeometryContainer
    {
    public:

      StaticGeometryContainer(util::Matrix<float, 4>& trfMatrix, util::ResourceHandle& meshHandle, util::ResourceHandle& materialHandle);
      ~StaticGeometryContainer();

      util::ResourceHandle getMaterialHandle();

      util::ResourceHandle getMeshHandle();

      util::Matrix<float, 4> getTransformationMatrix();

    protected:

      util::Matrix<float, 4>& m_trfMatrix;

      util::ResourceHandle& m_materialHandle;
      util::ResourceHandle& m_meshHandle;
    };
  }
}

#endif