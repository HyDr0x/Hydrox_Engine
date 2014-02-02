#ifndef GEOMETRYRASTERIZER_H_
#define GEOMETRYRASTERIZER_H_

#include <vector>

#include <Utilities/Math/Math.hpp>

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
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
}

#endif
