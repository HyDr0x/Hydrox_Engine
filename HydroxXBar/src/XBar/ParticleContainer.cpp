#include "XBar/ParticleContainer.h"

namespace he
{
	namespace xBar
	{
    ParticleContainer::ParticleContainer()
    {
    }

    ParticleContainer::~ParticleContainer()
    {
    }

    ParticleContainer* ParticleContainer::clone()
    {
      return new ParticleContainer();
    }

    bool ParticleContainer::operator ==(const ParticleContainer& o)
    {
      return true;
    }

    util::Matrix<float, 4> ParticleContainer::getTransformationMatrix()
    {
      return m_trfMatrix;
    }
  }
}