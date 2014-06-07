#ifndef PARTICLEEMITTER_H_
#define PARTICLEEMITTER_H_

#include "Renderer/DLLExport.h"

#include "Renderer/Resources/ManagedResource.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI ParticleEmitter : public ManagedResource
    {
    public:

      void free();

    private:
    };

  }
}

#endif
