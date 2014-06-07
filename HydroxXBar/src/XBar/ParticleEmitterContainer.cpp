#include "XBar/ParticleEmitterContainer.h"

namespace he
{
  namespace xBar
  {
    ParticleEmitterContainer::ParticleEmitterContainer(util::Matrix<float, 4> *trfMatrix) : m_trfMatrix(trfMatrix)
    {
      createHash();
    }

    ParticleEmitterContainer::~ParticleEmitterContainer()
    {
    }

    bool ParticleEmitterContainer::operator == (const ParticleEmitterContainer& other) const
    {
      return m_hash == other.m_hash;
    }

    util::Matrix<float, 4> ParticleEmitterContainer::getTransformationMatrix() const
    {
      return *m_trfMatrix;
    }

    void ParticleEmitterContainer::createHash()
    {
      std::vector<unsigned char> data(sizeof(m_trfMatrix));

      unsigned int offset = 0;
      offset = convertToRawData(m_trfMatrix, &data[0], offset);

      m_hash = MurmurHash64A(&data[0], data.size(), 0);
    }
  }
}