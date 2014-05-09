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
      createHash();
    }

    StaticGeometryContainer::~StaticGeometryContainer()
    {
    }

    bool StaticGeometryContainer::operator == (const StaticGeometryContainer& o) const
    {
      return m_hash == o.m_hash;
    }

    util::ResourceHandle StaticGeometryContainer::getMaterialHandle() const
    {
      return m_materialHandle;
    }

    util::ResourceHandle StaticGeometryContainer::getMeshHandle() const
    {
      return m_meshHandle;
    }

    util::Matrix<float, 4> StaticGeometryContainer::getTransformationMatrix() const
    {
      return *m_trfMatrix;
    }

    void StaticGeometryContainer::createHash()
    {
      unsigned int id;

      std::vector<unsigned char> data(sizeof(m_trfMatrix) + sizeof(id) + sizeof(id));

      std::copy((unsigned char*)&m_trfMatrix, (unsigned char*)&m_trfMatrix + sizeof(m_trfMatrix), &data[0]);

      id = m_materialHandle.getID();

      std::copy((unsigned char*)&id, (unsigned char*)&id + sizeof(id), &data[0] + sizeof(m_trfMatrix));

      id = m_meshHandle.getID();

      std::copy((unsigned char*)&id, (unsigned char*)&id + sizeof(id), &data[0] + sizeof(m_trfMatrix) + sizeof(id));

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}