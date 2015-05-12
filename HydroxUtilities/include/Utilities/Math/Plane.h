#ifndef PLANE_H_
#define PLANE_H_

#include <vector>

#include "Utilities/Math/math.hpp"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI Plane
    {
    public:

      Plane();
      Plane(vec3f v0, vec3f v1, vec3f v2);
      Plane(vec3f v0, vec3f normal);

      vec3f projectPointOn(vec3f p);

      bool collisionPlane(const Plane& plane, vec3f& outPos, vec3f& outDirection) const;
      bool collisionLine(vec3f pos, vec3f direction, vec3f& output) const;
      bool collisionCenteredBox(vec3f boxCenter, vec3f boxHalfSize, float epsilon) const;
      bool isPointBehind(vec3f v, float epsilon = 0.0f) const;

      vec3f getPosition() const;
      void setPosition(vec3f pos);

      vec3f getEdge0() const;
      void setEdge0(vec3f edge);

      vec3f getEdge1() const;
      void setEdge1(vec3f edge);

      vec3f getNormal() const;

    private:

      vec3f m_pos, m_edge0, m_edge1;//EDGES MUST NOT BE NORMALIZED!
      vec3f m_normal;
    };
  }
}

#endif