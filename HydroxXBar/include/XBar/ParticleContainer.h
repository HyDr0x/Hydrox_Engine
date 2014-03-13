#ifndef PARTICLECONTAINER_H_
#define PARTICLECONTAINER_H_

#include <Utilities/Math/Math.hpp>

#include "XBar/DLLExport.h"

namespace he
{
	namespace xBar
	{
    class GRAPHICAPI ParticleContainer
    {
    public:

      ParticleContainer();
      ~ParticleContainer();

      ParticleContainer* clone();

      bool operator ==(const ParticleContainer& o);

      util::Matrix<float, 4> getTransformationMatrix();

    private:

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix
    };
  }
}

#endif