#include "XBar/StaticGeometryContainer.h"

namespace he
{
  namespace xBar
  {
    StaticGeometryContainer::StaticGeometryContainer(util::Matrix<float, 4> *trfMatrix, util::ResourceHandle materialHandle, util::ResourceHandle meshHandle) : 
      IGeometryContainer(trfMatrix, materialHandle, meshHandle)
    {
      createHash();
    }

    StaticGeometryContainer::~StaticGeometryContainer()
    {
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