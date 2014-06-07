#ifndef PARTICLEEMITTERCONTAINER_H_
#define PARTICLEEMITTERCONTAINER_H_

#include <Utilities/Math/Math.hpp>

#include "XBar/DLLExport.h"

#include "XBar/AContainer.h"

namespace he
{
  namespace xBar
  {
    class GRAPHICAPI ParticleEmitterContainer : public AContainer
    {
    public:

      ParticleEmitterContainer(util::Matrix<float, 4> *trfMatrix);
      ~ParticleEmitterContainer();

      bool operator == (const ParticleEmitterContainer& other) const;

      util::Matrix<float, 4> getTransformationMatrix() const;

    private:

      void createHash();

      util::Matrix<float, 4> *m_trfMatrix;//the tranformation util::Matrix
    };
  }
}

#endif