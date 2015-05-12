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

      util::vec3f difference(m_bbMax - m_bbMin);
      float epsilon = 0.0001f;

      for(unsigned int j = 0; j < 3; j++)
      {
        if(m_bbMax[j] - m_bbMin[j] < epsilon)
        {
          m_bbMin[j] += -epsilon;
          m_bbMax[j] += epsilon;
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

    bool AABB::collisionLine(util::vec3f point, util::vec3f direction) const
    {
      util::vec3f difference = (m_bbMax - m_bbMin);
      util::vec3f vmin, vmax;

      util::vec3f translateBox = m_bbMin + 0.5f * difference;

      util::vec3f translatedBBMin = m_bbMin - translateBox;
      util::vec3f translatedBBMax = m_bbMax - translateBox;

      point -= translateBox;

      if(translatedBBMin < point && point < translatedBBMax)
      {
        return true;
      }

      if(direction[0] > 0)
      {
        if(direction[1] > 0)
        {
          if(direction[2] > 0)
          {
            vmin = translatedBBMin;
            vmax = translatedBBMax;
          }
          else
          {
            vmin = util::vec3f(translatedBBMin[0], translatedBBMin[1], translatedBBMin[2] + difference[2]);
            vmax = util::vec3f(translatedBBMax[0], translatedBBMax[1], translatedBBMax[2] - difference[2]);
          }
        }
        else
        {
          if(direction[2] > 0)
          {
            vmin = util::vec3f(translatedBBMin[0], translatedBBMin[1] + difference[1], translatedBBMin[2]);
            vmax = util::vec3f(translatedBBMax[0], translatedBBMax[1] - difference[1], translatedBBMax[2]);
          }
          else
          {
            vmin = util::vec3f(translatedBBMin[0], translatedBBMin[1] + difference[1], translatedBBMin[2] + difference[2]);
            vmax = util::vec3f(translatedBBMax[0], translatedBBMax[1] - difference[1], translatedBBMax[2] - difference[2]);
          }
        }
      }
      else
      {
        if(direction[1] > 0)
        {
          if(direction[2] > 0)
          {
            vmin = util::vec3f(translatedBBMin[0] + difference[0], translatedBBMin[1], translatedBBMin[2]);
            vmax = util::vec3f(translatedBBMax[0] - difference[0], translatedBBMax[1], translatedBBMax[2]);
          }
          else
          {
            vmin = util::vec3f(translatedBBMin[0] + difference[0], translatedBBMin[1], translatedBBMin[2] + difference[2]);
            vmax = util::vec3f(translatedBBMax[0] - difference[0], translatedBBMax[1], translatedBBMax[2] - difference[2]);
          }
        }
        else
        {
          if(direction[2] > 0)
          {
            vmin = util::vec3f(translatedBBMin[0] + difference[0], translatedBBMin[1] + difference[1], translatedBBMin[2]);
            vmax = util::vec3f(translatedBBMax[0] - difference[0], translatedBBMax[1] - difference[1], translatedBBMax[2]);
          }
          else
          {
            vmin = translatedBBMax;
            vmax = translatedBBMin;
          }
        }
      }

      util::vec3f reciprocalDirection(1.0f / (direction + 0.0001f));

      util::vec3f tn, tf;

      tn = (vmin - point) * reciprocalDirection;
      tf = (vmax - point) * reciprocalDirection;

      bool tmpRes = std::max<float>(std::max<float>(tn[0], tn[1]), tn[2]) <= std::min<float>(std::min<float>(tf[0], tf[1]), tf[2]);
      return std::max<float>(std::max<float>(tn[0], tn[1]), tn[2]) <= std::min<float>(std::min<float>(tf[0], tf[1]), tf[2]);
    }
  }
}