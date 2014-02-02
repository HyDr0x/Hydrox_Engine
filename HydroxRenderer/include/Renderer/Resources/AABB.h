#ifndef AABB_H_
#define AABB_H_

#include <vector>

#include <Utilities/Math/Math.hpp>

namespace he
{
	namespace renderer
	{
    class AABB
    {
    public:

      AABB();
      AABB(std::vector<util::Vector<float, 3>> positions);
      ~AABB();

      util::Vector<float, 3> getBBMax();
      util::Vector<float, 3> getBBMin();

    private:

      util::Vector<float, 3> m_bbMax;
      util::Vector<float, 3> m_bbMin;
    };
	}
}

#endif
