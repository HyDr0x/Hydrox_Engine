#ifndef PARTICLEEMITTER_H_
#define PARTICLEEMITTER_H_

#include "DataBase/DLLExport.h"

#include "DataBase/ManagedResource.h"

namespace he
{
  namespace db
  {
    class GRAPHICAPI ParticleEmitter : public ManagedResource
    {
    public:

      void free();

    protected:

      virtual void updateHash();

    private:
    };

  }
}

#endif
