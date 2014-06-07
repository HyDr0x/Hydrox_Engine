#ifndef AABB_H_
#define AABB_H_

#include "Renderer/DLLExport.h"

#include <vector>

#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI AABB
    {
    public:

      AABB();
      AABB(util::Vector<float, 3> min, util::Vector<float, 3> max);
      AABB(const std::vector<util::Vector<float, 3>>& positions);
      ~AABB();

      util::Vector<float, 3> getBBMax() const;
      util::Vector<float, 3> getBBMin() const;

    private:

      util::Vector<float, 3> m_bbMax;
      util::Vector<float, 3> m_bbMin;
    };
  }
}

#endif
