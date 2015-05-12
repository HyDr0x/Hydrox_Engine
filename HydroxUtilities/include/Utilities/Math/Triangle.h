#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <vector>

#include "Utilities/Math/math.hpp"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI Triangle
    {
    public:

      Triangle();

      Triangle(vec3f v0, vec3f v1, vec3f v2);

      bool collisionCenteredBoxTest(vec3f boxCenter, vec3f boxHalfSize, float epsilon) const;

      float calculatePointTriangleDistance(vec3f point, vec3f& nearestPoint);

      const vec3f& operator[] (unsigned int i) const;

      void setPosition(unsigned int i, vec3f v);

      vec3f getNormal() const;

      float getArea() const;

      bool collisionLine(vec3f point, vec3f direction, vec3f& output) const;

    private:

      vec3f m_v[3];
      vec3f m_normal;
      float m_area;
    };
  }
}

#endif