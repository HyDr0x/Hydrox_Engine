#include "XBar/ParticleContainer.h"

namespace he
{
	namespace xBar
	{
    ParticleContainer::ParticleContainer()
    {
      createHash();
    }

    ParticleContainer::~ParticleContainer()
    {
    }

    bool ParticleContainer::operator == (const ParticleContainer& o)
    {
      return m_hash == o.m_hash;
    }

    util::Matrix<float, 4> ParticleContainer::getTransformationMatrix()
    {
      return *m_trfMatrix;
    }

    void ParticleContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(m_trfMatrix));

      std::copy((unsigned char*)&m_trfMatrix, (unsigned char*)&m_trfMatrix + sizeof(m_trfMatrix), &data[0]);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}