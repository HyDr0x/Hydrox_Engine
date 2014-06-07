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

    bool StaticGeometryContainer::operator == (const StaticGeometryContainer& other) const
    {
      return m_hash == other.m_hash;
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

      unsigned int offset = 0;
      offset = convertToRawData(m_trfMatrix, &data[0], offset);
      id = m_materialHandle.getID();
      offset = convertToRawData(id, &data[0], offset);
      id = m_meshHandle.getID();
      offset = convertToRawData(id, &data[0], offset);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}