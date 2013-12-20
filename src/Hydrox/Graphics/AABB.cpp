#include "Hydrox/Graphics/AABB.h"

#include <float.h>

namespace he
{
  AABB::AABB()
  {
    m_bbMin = Vector<float, 3>(FLT_MAX, FLT_MAX, FLT_MAX);
    m_bbMax = Vector<float, 3>(FLT_MIN, FLT_MIN, FLT_MIN);
  }

  AABB::AABB(std::vector<Vector<float, 3>> positions)
  {
    m_bbMin = Vector<float, 3>(FLT_MAX, FLT_MAX, FLT_MAX);
    m_bbMax = Vector<float, 3>(FLT_MIN, FLT_MIN, FLT_MIN);

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

  Vector<float, 3> AABB::getBBMax()
  {
    return m_bbMax;
  }

  Vector<float, 3> AABB::getBBMin()
  {
    return m_bbMin;
  }
}