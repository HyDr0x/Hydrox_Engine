#include "Renderer/TreeNodes/GeometryContainer/StaticGeometryContainer.h"

namespace he
{
	namespace renderer
	{
    StaticGeometryContainer::StaticGeometryContainer(util::Matrix<float, 4>& trfMatrix, util::ResourceHandle& meshHandle, util::ResourceHandle& materialHandle) : 
      m_trfMatrix(trfMatrix),
      m_meshHandle(meshHandle),
      m_materialHandle(materialHandle)
    {
    }

    StaticGeometryContainer::~StaticGeometryContainer()
    {
    }

    util::ResourceHandle StaticGeometryContainer::getMaterialHandle()
    {
      return m_materialHandle;
    }

    util::ResourceHandle StaticGeometryContainer::getMeshHandle()
    {
      return m_meshHandle;
    }

    util::Matrix<float, 4> StaticGeometryContainer::getTransformationMatrix()
    {
      return m_trfMatrix;
    }
  }
}