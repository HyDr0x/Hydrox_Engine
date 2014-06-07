#include "XBar/LightContainer.h"

namespace he
{
  namespace xBar
  {
    LightContainer::LightContainer(util::Matrix<float, 4> *trfMatrix, util::ResourceHandle lightHandle)
    {
      createHash();
    }

    LightContainer::~LightContainer()
    {
    }

    bool LightContainer::operator == (const LightContainer& other) const
    {
      return m_hash == other.m_hash;
    }

    util::ResourceHandle LightContainer::getLightHandle() const
    {
      return m_lightHandle;
    }

    util::Matrix<float, 4> LightContainer::getTransformationMatrix() const
    {
      return *m_trfMatrix;
    }

    void LightContainer::createHash()
    {
      unsigned int id = m_lightHandle.getID();

      std::vector<unsigned char> data(sizeof(m_trfMatrix)+sizeof(id));

      unsigned int offset = 0;

      offset = convertToRawData(id, &data[0], offset);
      offset = convertToRawData(m_trfMatrix, &data[0], offset);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}