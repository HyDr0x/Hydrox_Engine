#include "Renderer/Resources/AABB.h"

#include <float.h>

namespace he
{
	namespace renderer
	{
	

    AABB::AABB()
    {
      m_bbMin = util::Vector<float, 3>(FLT_MAX, FLT_MAX, FLT_MAX);
      m_bbMax = util::Vector<float, 3>(FLT_MIN, FLT_MIN, FLT_MIN);
    }

    AABB::AABB(const std::vector<util::Vector<float, 3>>& positions)
    {
      m_bbMin = util::Vector<float, 3>(FLT_MAX, FLT_MAX, FLT_MAX);
      m_bbMax = util::Vector<float, 3>(FLT_MIN, FLT_MIN, FLT_MIN);

      for(unsigned int i = 0; i < positions.size(); i++)
      {
        for(unsigned int j = 0; j < 3; j++)
        {
          if(m_bbMin[j] > positions[i][j])
          {
            m_bbMin[j] = positions[i][j];
          }

          if(m_bbMax[j] < positions[i][j])
          {
            m_bbMax[j] = positions[i][j];
          }
        }
      }
    }

    AABB::~AABB()
    {

    }

    util::Vector<float, 3> AABB::getBBMax() const
    {
      return m_bbMax;
    }

    util::Vector<float, 3> AABB::getBBMin() const
    {
      return m_bbMin;
    }

	}
}