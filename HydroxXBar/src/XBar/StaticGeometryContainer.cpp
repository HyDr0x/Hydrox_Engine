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

    void StaticGeometryContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(void*) + sizeof(unsigned int) * 2);

      unsigned int id;

      memcpy(&data[0], m_trfMatrix, sizeof(void*));
      id = m_materialHandle.getID();
      memcpy(&data[sizeof(void*)], &id, sizeof(unsigned int));
      id = m_meshHandle.getID();
      memcpy(&data[sizeof(void*) + sizeof(unsigned int)], &id, sizeof(unsigned int));

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}