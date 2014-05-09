#include "XBar/LightContainer.h"

namespace he
{
	namespace xBar
	{
    LightContainer::LightContainer()
    {
      createHash();
    }

    LightContainer::~LightContainer()
    {
    }

    bool LightContainer::operator == (const LightContainer& o) const
    {
      return m_hash == o.m_hash;
    }

    util::Matrix<float, 4> LightContainer::getTransformationMatrix() const
    {
      return *m_trfMatrix;
    }

    void LightContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(m_trfMatrix));

      std::copy((unsigned char*)&m_trfMatrix, (unsigned char*)&m_trfMatrix + sizeof(m_trfMatrix), &data[0]);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}