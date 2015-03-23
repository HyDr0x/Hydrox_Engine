#include "Utilities/Math/Triangle.h"

#include <cassert>

#include <math.h>

#include "Utilities/Math/Plane.h"

namespace he
{
  namespace util
  {
    Triangle::Triangle(vec3f v0, vec3f v1, vec3f v2)
    {
      m_v[0] = v0;
      m_v[1] = v1;
      m_v[2] = v2;

      m_normal = math::cross(m_v[1] - m_v[0], m_v[2] - m_v[0]).normalize();
    }

    const vec3f& Triangle::operator[] (unsigned int i) const
    {
      assert(i < 3);

      return m_v[i];
    }

    void Triangle::setPosition(unsigned int i, vec3f v)
    {
      assert(i < 3);

      m_v[i] = v;

      m_normal = math::cross(m_v[1] - m_v[0], m_v[2] - m_v[0]).normalize();
    }

    vec3f Triangle::getNormal() const
    {
      return m_normal;
    }

    float Triangle::getArea() const
    {
      vec3f e0 = m_v[1] - m_v[0];
      vec3f e1 = m_v[2] - m_v[0];

      return math::cross(e0, e1).length() * 0.5f;
    }

    static bool testAABBEdgeWithTriangle(vec3f v0, vec3f v1, vec3f v2, float boxHalfSize, vec3f e)
    {
      float p0, p1, p2;
      float vmin, vmax;

      p0 = vec3f::dot(v0, e);
      p1 = vec3f::dot(v1, e);
      p2 = vec3f::dot(v2, e);

      vmin = std::min(p0, std::min(p1, p2));
      vmax = std::max(p0, std::max(p1, p2));

      if(vmin > boxHalfSize || vmax < -boxHalfSize) return false;//no collision

      return true;
    }

    static bool testTriangleEdgeWithAABB(vec3f t0, vec3f t1, vec3f t2, vec3f boxHalfSize, vec3f e)
    {
      //Project the AABB onto the triangle edge
      vec3f v0 = vec3f(-boxHalfSize[0], -boxHalfSize[1], -boxHalfSize[2]);
      vec3f v1 = vec3f(boxHalfSize[0], -boxHalfSize[1], -boxHalfSize[2]);
      vec3f v2 = vec3f(-boxHalfSize[0], -boxHalfSize[1], boxHalfSize[2]);
      vec3f v3 = vec3f(boxHalfSize[0], -boxHalfSize[1], boxHalfSize[2]);

      vec3f v4 = vec3f(-boxHalfSize[0], boxHalfSize[1], -boxHalfSize[2]);
      vec3f v5 = vec3f(boxHalfSize[0], boxHalfSize[1], -boxHalfSize[2]);
      vec3f v6 = vec3f(-boxHalfSize[0], boxHalfSize[1], boxHalfSize[2]);
      vec3f v7 = vec3f(boxHalfSize[0], boxHalfSize[1], boxHalfSize[2]);

      float p0, p1, p2, p3, p4, p5, p6, p7;
      float vmin, vmax;

      p0 = vec3f::dot(v0, e);
      p1 = vec3f::dot(v1, e);
      p2 = vec3f::dot(v2, e);
      p3 = vec3f::dot(v3, e);
      p4 = vec3f::dot(v4, e);
      p5 = vec3f::dot(v5, e);
      p6 = vec3f::dot(v6, e);
      p7 = vec3f::dot(v7, e);

      vmin = std::min(p0, std::min(p1, std::min(p2, std::min(p3, std::min(p4, std::min(p5, std::min(p6, p7)))))));
      vmax = std::max(p0, std::max(p1, std::max(p2, std::max(p3, std::max(p4, std::max(p5, std::max(p6, p7)))))));

      /////////Project the triangle onto its own edge
      p0 = vec3f::dot(t0, e);
      p1 = vec3f::dot(t1, e);
      p2 = vec3f::dot(t2, e);

      float vmin2 = std::min(p0, std::min(p1, p2));
      float vmax2 = std::max(p0, std::max(p1, p2));

      if(vmin > vmax2 || vmax < vmin2) return false;//no collision

      return true;
    }

    static bool testXAxis(vec3f v0, vec3f v1, float a, float b, vec3f boxHalfSize)
    {
      float p0, p1;

      p0 = a * v0[1] - b * v0[2];//vec3f::dot(v0, f);
      p1 = a * v1[1] - b * v1[2];//vec3f::dot(v1, f);

      float min, max;

      if(p0 < p1)
      {
        min = p0;
        max = p1;
      }
      else
      {
        min = p1;
        max = p0;
      }

      float r = fabsf(a) * boxHalfSize[1] + fabsf(b) * boxHalfSize[2];
      if(min > r || max < -r) return false;

      return true;
    }

    static bool testYAxis(vec3f v0, vec3f v1, float a, float b, vec3f boxHalfSize)
    {
      float p0, p1;

      p0 = -a * v0[0] + b * v0[2];//vec3f::dot(v0, f);
      p1 = -a * v1[0] + b * v1[2];//vec3f::dot(v1, f);

      float min, max;

      if(p0 < p1)
      {
        min = p0;
        max = p1;
      }
      else
      {
        min = p1;
        max = p0;
      }

      float r = fabsf(a) * boxHalfSize[0] + fabsf(b) * boxHalfSize[2];
      if(min > r || max < -r) return false;

      return true;
    }

    static bool testZAxis(vec3f v0, vec3f v1, float a, float b, vec3f boxHalfSize)
    {
      float p0, p1;

      p0 = a * v0[0] - b * v0[1];//vec3f::dot(v0, f);
      p1 = a * v1[0] - b * v1[1];//vec3f::dot(v1, f);

      float min, max;

      if(p0 < p1)
      {
        min = p0;
        max = p1;
      }
      else
      {
        min = p1;
        max = p0;
      }

      float r = fabsf(a) * boxHalfSize[0] + fabsf(b) * boxHalfSize[1];
      if(min > r || max < -r) return false;

      return true;
    }

    static void findMinMax(float a, float b, float c, float& min, float& max)
    {
      min = max = a;

      if(b < min) min = b;
      if(b > max) max = b;

      if(c < min) min = c;
      if(c > max) max = c;
    }

    bool Triangle::collisionCenteredBoxTest(vec3f boxCenter, vec3f boxHalfSize) const
    {
      vec3f v0(m_v[0] - boxCenter), v1(m_v[1] - boxCenter), v2(m_v[2] - boxCenter);

      float vmin, vmax;

      findMinMax(v0[0], v1[0], v2[0], vmin, vmax);
      if(vmin > boxHalfSize[0] || vmax < -boxHalfSize[0]) return false;

      findMinMax(v0[1], v1[1], v2[1], vmin, vmax);
      if(vmin > boxHalfSize[1] || vmax < -boxHalfSize[1]) return false;

      findMinMax(v0[2], v1[2], v2[2], vmin, vmax);
      if(vmin > boxHalfSize[2] || vmax < -boxHalfSize[2]) return false;

      Plane plane(v0, v1, v2);
      if(!plane.collisionCenteredBox(vec3f(0, 0, 0), boxHalfSize)) return false;//zero vector because its already centered

      vec3f e[3];

      e[0] = v1 - v0;
      e[1] = v2 - v1;
      e[2] = v0 - v2;

      if(!testXAxis(v0, v2, e[0][2], e[0][1], boxHalfSize)) return false;
      if(!testXAxis(v0, v2, e[1][2], e[1][1], boxHalfSize)) return false;
      if(!testXAxis(v0, v1, e[2][2], e[2][1], boxHalfSize)) return false;
      //if(!testXAxis(v1, v2, e[2], boxHalfSize)) return false;

      if(!testYAxis(v0, v2, e[0][2], e[0][0], boxHalfSize)) return false;
      if(!testYAxis(v0, v2, e[1][2], e[1][0], boxHalfSize)) return false;
      if(!testYAxis(v0, v1, e[2][2], e[2][0], boxHalfSize)) return false;
      //if(!testYAxis(v1, v2, e[2], boxHalfSize)) return false;

      if(!testZAxis(v1, v2, e[0][1], e[0][0], boxHalfSize)) return false;
      if(!testZAxis(v1, v2, e[1][1], e[1][0], boxHalfSize)) return false;
      if(!testZAxis(v0, v1, e[2][1], e[2][0], boxHalfSize)) return false;
      //if(!testZAxis(v0, v2, e[0], boxHalfSize)) return false;
      //if(!testZAxis(v0, v2, e[1], boxHalfSize)) return false;
      //if(!testZAxis(v1, v2, e[2], boxHalfSize)) return false;

      return true;
    }
  }
}