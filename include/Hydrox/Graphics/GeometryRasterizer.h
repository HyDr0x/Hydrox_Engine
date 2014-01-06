#ifndef GEOMETRYRASTERIZER_H_
#define GEOMETRYRASTERIZER_H_

#include <vector>

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Graphics/Mesh.h"

namespace he
{
  class GeometryRasterizer
  {
    public:

      GeometryRasterizer();
      ~GeometryRasterizer();

      void rasterizeGeometry();

    private:

      std::vector<Mesh> m_geometry;
  };
}

#endif
