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
      std::vector<unsigned char> data(sizeof(void*));

      memcpy(&data[0], m_trfMatrix, sizeof(void*));

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}