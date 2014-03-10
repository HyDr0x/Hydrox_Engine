#ifndef SKINNEDGEOMETRYCONTAINER_H_
#define SKINNEDGEOMETRYCONTAINER_H_

#include <vector>

#include "Renderer/TreeNodes/GeometryContainer/StaticGeometryContainer.h"

namespace he
{
	namespace renderer
	{
    class SkinnedGeometryContainer : public StaticGeometryContainer
    {
    public:

      SkinnedGeometryContainer(util::Matrix<float, 4>& trfMatrix, util::ResourceHandle& meshHandle, util::ResourceHandle& materialHandle, std::vector<util::Matrix<float, 4>>& skinningMatrices);
      ~SkinnedGeometryContainer();

      std::vector<util::Matrix<float, 4>> getSkinningMatrices();

    private:

      std::vector<util::Matrix<float, 4>>& m_skinningMatrices;
    };
  }
}

#endif