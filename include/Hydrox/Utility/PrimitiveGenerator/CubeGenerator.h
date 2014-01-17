#ifndef CUBEGENERATOR_H_
#define CUBEGENERATOR_H_

#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"
#include "Hydrox/Graphics/Mesh.h"

namespace he
{
  class CubeGenerator
  {
  public:

    CubeGenerator();
    ~CubeGenerator();

    static Mesh generateCube();

  private:

  };
}
#endif
