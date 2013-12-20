#ifndef AABB_H_
#define AABB_H_

#include <vector>

#include "Hydrox/Utility/Math/Math.hpp"

namespace he
{
  class AABB
  {
  public:

    AABB();
    AABB(std::vector<Vector<float, 3>> positions);
    ~AABB();

    Vector<float, 3> getBBMax();
    Vector<float, 3> getBBMin();

  private:

    Vector<float, 3> m_bbMax;
    Vector<float, 3> m_bbMin;
  };
}

#endif