#include "Renderer/TreeNodes/GeometryContainer/SkinnedGeometryContainer.h"

namespace he
{
	namespace renderer
	{
    SkinnedGeometryContainer::SkinnedGeometryContainer(util::Matrix<float, 4>& trfMatrix, util::ResourceHandle& meshHandle, util::ResourceHandle& materialHandle, std::vector<util::Matrix<float, 4>>& skinningMatrices) : 
      StaticGeometryContainer(trfMatrix, meshHandle, materialHandle), 
      m_skinningMatrices(skinningMatrices)
    {
    }

    SkinnedGeometryContainer::~SkinnedGeometryContainer()
    {
    }

    std::vector<util::Matrix<float, 4>> SkinnedGeometryContainer::getSkinningMatrices()
    {
      return m_skinningMatrices;
    }
  }
}