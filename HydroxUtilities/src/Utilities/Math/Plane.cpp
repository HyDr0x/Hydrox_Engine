#include "Utilities/Math/Plane.h"

#include <cassert>

#include <math.h>

namespace he
{
  namespace util
  {
    Plane::Plane()
    {
      m_pos = vec3f::identity();
      m_edge0 = vec3f::identity();
      m_edge1 = vec3f::identity();

      m_normal = vec3f::identity();
    }

    Plane::Plane(vec3f v0, vec3f v1, vec3f v2) :
      m_pos(v0)
    {
      m_edge0 = (v1 - m_pos);
      m_edge1 = (v2 - m_pos);

      m_normal = math::cross(m_edge0, m_edge1).normalize();
    }

    Plane::Plane(vec3f v0, vec3f normal) :
      m_pos(v0),
      m_normal(normal.normalize())
    {
      assert(math::cross(m_normal, vec3f(1, 2, 3)) != vec3f::identity());

      m_edge0 = math::cross(m_normal, vec3f(1, 2, 3));
      m_edge1 = math::cross(m_normal, m_edge0);
    }

    vec3f Plane::projectPointOn(vec3f p)
    {
      vec3f output;
      collisionLine(p, m_normal, output);

      return p + output[0] * m_normal;
    }

    bool Plane::collisionPlane(const Plane& plane, vec3f& outPos, vec3f& outDirection) const
    {
      outDirection = math::cross(m_normal, plane.getNormal()).normalize();

      float d0 = vec3f::dot(m_pos, m_normal);
      float d1 = vec3f::dot(plane.getPosition(), plane.getNormal());

      //set x, y or z = 1, so we can never be in the x, y, or z plane, cause we shifted the box around +- 0.5f
      if(m_normal != vec3f(1.0f, 0.0f, 0.0f) && plane.getNormal() != vec3f(1.0f, 0.0f, 0.0f))//are we in the x plane?
      {
        outPos[0] = 1.0f;

        float det = m_normal[1] * plane.getNormal()[2] - m_normal[2] * plane.getNormal()[1];

        outPos[1] = (d0 * plane.getNormal()[2] - m_normal[2] * d1) / det;
        outPos[2] = (m_normal[1] * d1 - d0 * plane.getNormal()[1]) / det;
      }
      else if(m_normal != vec3f(0.0f, 1.0f, 0.0f) && plane.getNormal() != vec3f(0.0f, 1.0f, 0.0f))//are we in the y plane ?
      {
        outPos[1] = 1.0f;

        float det = m_normal[0] * plane.getNormal()[2] - m_normal[2] * plane.getNormal()[0];

        outPos[0] = (d0 * plane.getNormal()[2] - m_normal[2] * d1) / det;
        outPos[2] = (m_normal[0] * d1 - d0 * plane.getNormal()[0]) / det;
      }
      else if(m_normal != vec3f(0.0f, 0.0f, 1.0f) && plane.getNormal() != vec3f(0.0f, 0.0f, 1.0f))//are we in the z plane?
      {
        outPos[2] = 1.0f;

        float det = m_normal[0] * plane.getNormal()[1] - m_normal[1] * plane.getNormal()[0];

        outPos[0] = (d0 * plane.getNormal()[1] - m_normal[1] * d1) / det;
        outPos[1] = (m_normal[0] * d1 - d0 * plane.getNormal()[0]) / det;
      }
      else//no collision!
      {
        outPos = vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
        return false;
      }

      return true;
    }

    bool Plane::collisionLine(vec3f pos, vec3f direction, vec3f& output) const
    {
      vec3f w = pos - m_pos;

      vec3f dv = math::cross(direction, m_edge1);
      vec3f wu = math::cross(w, m_edge0);

      float denominator = vec3f::dot(dv, m_edge0);

      if(denominator)
      {
        output = 1.0f / denominator * vec3f(vec3f::dot(wu, m_edge1), vec3f::dot(dv, w), vec3f::dot(wu, direction));

        return true;
      }

      return false;
    }

    bool Plane::collisionCenteredBox(vec3f boxCenter, vec3f boxHalfSize, float epsilon) const
    {
      vec3f centeredPos = m_pos - boxCenter;

      vec3f vmax, vmin;

      for(unsigned int i = 0; i < 3; i++)
      {
        if(m_normal[i] > 0.0f)
        {
          vmin[i] = -boxHalfSize[i] - centeredPos[i];
          vmax[i] = boxHalfSize[i] - centeredPos[i];
        }
        else
        {
          vmin[i] = boxHalfSize[i] - centeredPos[i];
          vmax[i] = -boxHalfSize[i] - centeredPos[i];
        }
      }

      if(vec3f::dot(m_normal, vmin) > epsilon) return false;
      if(vec3f::dot(m_normal, vmax) >= -epsilon) return true;

      return false;
    }

    bool Plane::isPointBehind(vec3f v, float epsilon) const
    {
      return vec3f::dot(m_normal, (v - m_pos)) < epsilon;
    }

    vec3f Plane::getPosition() const
    {
      return m_pos;
    }

    void Plane::setPosition(vec3f pos)
    {
      m_pos = pos;
    }

    vec3f Plane::getEdge0() const
    {
      return m_edge0;
    }

    void Plane::setEdge0(vec3f edge)
    {
      m_edge0 = edge;

      m_normal = math::cross(m_edge0, m_edge1).normalize();
    }

    vec3f Plane::getEdge1() const
    {
      return m_edge1;
    }

    void Plane::setEdge1(vec3f edge)
    {
      m_edge1 = edge;

      m_normal = math::cross(m_edge0, m_edge1).normalize();
    }

    vec3f Plane::getNormal() const
    {
      return m_normal;
    }
  }
}