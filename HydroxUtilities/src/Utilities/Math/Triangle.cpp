#include "Utilities/Math/Triangle.h"

#include <cassert>

#include <cmath>

#include "Utilities/Math/Plane.h"

namespace he
{
  namespace util
  {
    Triangle::Triangle()
    {
      m_v[0] = m_v[1] = m_v[2] = m_normal = vec3f::identity();      
    }

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

    float Triangle::calculatePointTriangleDistance(vec3f point, vec3f& nearestPoint)
    {
      vec3f pointPlanePos;

      Plane plane(m_v[0], m_v[1], m_v[2]);
      plane.collisionLine(point, m_normal, pointPlanePos);

      if(0.0f <= pointPlanePos[1] && pointPlanePos[1] <= 1.0f && 0.0f <= pointPlanePos[2] && pointPlanePos[2] <= 1.0f && pointPlanePos[1] + pointPlanePos[2] <= 1.0f)
      {
        vec3f direction = pointPlanePos[0] * m_normal;
        nearestPoint = point + direction;
        return direction.length();//point lies, projected on the triangle plane, in the triangle
      }
      else
      {
        vec3f edge, pivotPoint;;

        unsigned int index;

        float distance = FLT_MAX;
        float pointProj;

        for(unsigned int i = 0; i < 3; i++)
        {
          index = (i + 1) % 3;

          edge = m_v[index] - m_v[i];
          float edgeLength = edge.length();
          edge = edge.normalize();

          /* lineProj0 = vec3f::dot(m_points[i] - m_points[i], edge);
          lineProj1 = vec3f::dot(m_points[index] - m_points[i], edge);*/
          pointProj = vec3f::dot(point - m_v[i], edge);

          if(0 < pointProj && pointProj < edgeLength)//is the projection on the line between the polygon points which spans the line?
          {
            pivotPoint = m_v[i] + pointProj * edge;
          }
          else if(pointProj < 0)
          {
            pivotPoint = m_v[i];
          }
          else
          {
            pivotPoint = m_v[index];
          }

          float tmpDistance = (pivotPoint - point).length();
          if(distance > tmpDistance)
          {
            nearestPoint = pivotPoint;
            distance = tmpDistance;
          }
        }

        return distance;
      }
    }

    bool Triangle::collisionLine(vec3f point, vec3f direction, vec3f& output) const
    {
      vec3f e0 = m_v[1] - m_v[0];
      vec3f e1 = m_v[2] - m_v[0];

      /* LGS Cramersche Regel*/
      float r = m_v[0][0] - point[0];
      float s = m_v[0][1] - point[1];
      float t = m_v[0][2] - point[2];
      Matrix<float, 3> a(e0, e1, direction);

      float det = 1.0f / a.detMat();
      a[0][0] = r;
      a[0][1] = s;
      a[0][2] = t;
      float u = a.detMat();

      a[0][0] = e0[0];
      a[0][1] = e0[1];
      a[0][2] = e0[2];
      a[1][0] = r;
      a[1][1] = s;
      a[1][2] = t;
      float v = a.detMat();

      a[1][0] = e1[0];
      a[1][1] = e1[1];
      a[1][2] = e1[2];
      a[2][0] = r;
      a[2][1] = s;
      a[2][2] = t;
      float w = a.detMat();

      r = u * det;
      s = v * det;
      t = w * det;

      output = vec3f(t, r, s);

      return ((s + r) >= -1.0f && (s + r) <= 0.0f && s <= 0.0f && r <= 0.0f && s >= -1.0f && r >= -1.0f) && (t > 0.00001f /*|| Skalar(m_n,*cv)+Skalar(m_n,(*cv)+(*rv))<0*/);
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
      if(math::floatSafeGreater(min, r) || math::floatSafeGreater(-r, max)) return false;

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
      if(math::floatSafeGreater(min, r) || math::floatSafeGreater(-r, max)) return false;

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
      if(math::floatSafeGreater(min, r) || math::floatSafeGreater(-r, max)) return false;

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

    bool Triangle::collisionCenteredBoxTest(vec3f boxCenter, vec3f boxHalfSize, float epsilon) const
    {
      vec3f v0(m_v[0] - boxCenter), v1(m_v[1] - boxCenter), v2(m_v[2] - boxCenter);

      float vmin, vmax;

      findMinMax(v0[0], v1[0], v2[0], vmin, vmax);
      if(math::floatSafeGreater(vmin, boxHalfSize[0]) || math::floatSafeGreater(-boxHalfSize[0], vmax)) return false;

      findMinMax(v0[1], v1[1], v2[1], vmin, vmax);
      if(math::floatSafeGreater(vmin, boxHalfSize[1]) || math::floatSafeGreater(-boxHalfSize[1], vmax)) return false;

      findMinMax(v0[2], v1[2], v2[2], vmin, vmax);
      if(math::floatSafeGreater(vmin, boxHalfSize[2]) || math::floatSafeGreater(-boxHalfSize[2], vmax)) return false;

      Plane plane(v0, v1, v2);
      if(!plane.collisionCenteredBox(vec3f(0, 0, 0), boxHalfSize, epsilon)) return false;//zero vector because its already centered

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