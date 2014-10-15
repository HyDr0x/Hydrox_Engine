#include "DataBase/AABB.h"

#include <float.h>

namespace he
{
  namespace db
  {
    AABB::AABB()
    {
      m_bbMin = util::vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
      m_bbMax = util::vec3f(FLT_MIN, FLT_MIN, FLT_MIN);
    }

    AABB::AABB(util::vec3f min, util::vec3f max) : m_bbMax(max), m_bbMin(min)
    {
    }

    AABB::AABB(const std::vector<util::vec3f>& positions)
    {
      m_bbMin = util::vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
      m_bbMax = util::vec3f(FLT_MIN, FLT_MIN, FLT_MIN);

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

    util::vec3f AABB::getBBMax() const
    {
      return m_bbMax;
    }

    util::vec3f AABB::getBBMin() const
    {
      return m_bbMin;
    }

  }
}