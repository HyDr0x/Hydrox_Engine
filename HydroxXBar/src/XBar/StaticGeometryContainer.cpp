#include "XBar/StaticGeometryContainer.h"

namespace he
{
	namespace xBar
	{
    StaticGeometryContainer::StaticGeometryContainer(util::Matrix<float, 4> *trfMatrix, util::ResourceHandle materialHandle, util::ResourceHandle meshHandle) : 
      m_trfMatrix(trfMatrix),
      m_meshHandle(meshHandle),
      m_materialHandle(materialHandle)
    {
    }

    StaticGeometryContainer::~StaticGeometryContainer()
    {
    }

    StaticGeometryContainer* StaticGeometryContainer::clone()
    {
      return new StaticGeometryContainer(m_trfMatrix, m_materialHandle, m_meshHandle);
    }

    bool StaticGeometryContainer::operator ==(const StaticGeometryContainer& o)
    {
      return m_materialHandle == o.m_materialHandle && m_meshHandle == o.m_meshHandle && m_trfMatrix == o.m_trfMatrix;
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
      return *m_trfMatrix;
    }
  }
}