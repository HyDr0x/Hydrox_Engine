#include "Utilities/Math/CollisionTests.h"

#include <math.h>

#include "Utilities/Math/math.hpp"
#include "Utilities/Math/Quaternion.hpp"

namespace he
{
  namespace util
  {
    bool planePlaneCollison(vec3f supportVector0, vec3f normal0, vec3f supportVector1, vec3f normal1, vec3f& outPos, vec3f& outDirection)
    {
      outDirection = math::cross(normal0, normal1).normalize();

      float d0 = vec3f::dot(supportVector0, normal0);
      float d1 = vec3f::dot(supportVector1, normal1);

      //set x, y or z = 1, so we can never be in the x, y, or z plane, cause we shifted the box around +- 0.5f
      if(normal0 != vec3f(1.0f, 0.0f, 0.0f) && normal1 != vec3f(1.0f, 0.0f, 0.0f))//are we in the x plane?
      {
        outPos[0] = 1.0f;

        float det = normal0[1] * normal1[2] - normal0[2] * normal1[1];

        outPos[1] = (d0 * normal1[2] - normal0[2] * d1) / det;
        outPos[2] = (normal0[1] * d1 - d0 * normal1[1]) / det;
      }
      else if(normal0 != vec3f(0.0f, 1.0f, 0.0f) && normal1 != vec3f(0.0f, 1.0f, 0.0f))//are we in the y plane ?
      {
        outPos[1] = 1.0f;

        float det = normal0[0] * normal1[2] - normal0[2] * normal1[0];

        outPos[0] = (d0 * normal1[2] - normal0[2] * d1) / det;
        outPos[2] = (normal0[0] * d1 - d0 * normal1[0]) / det;
      }
      else if(normal0 != vec3f(0.0f, 0.0f, 1.0f) && normal1 != vec3f(0.0f, 0.0f, 1.0f))//are we in the z plane?
      {
        outPos[2] = 1.0f;

        float det = normal0[0] * normal1[1] - normal0[1] * normal1[0];

        outPos[0] = (d0 * normal1[1] - normal0[1] * d1) / det;
        outPos[1] = (normal0[0] * d1 - d0 * normal1[0]) / det;
      }
      else//no collision!
      {
        outPos = vec3f(UINT_MAX, UINT_MAX, UINT_MAX);
        return false;
      }

      return true;
    }

    bool linePlaneCollision(vec3f pos0, vec3f direction0, vec3f v0, vec3f v1, vec3f v2, vec3f& output)
    {
      vec3f u = v1 - v0;
      vec3f v = v2 - v0;
      vec3f w = pos0 - v0;

      vec3f dv = math::cross(direction0, v);
      vec3f wu = math::cross(w, u);

      float denominator = vec3f::dot(dv, u);

      if(denominator)
      {
        output = 1.0f / denominator * vec3f(vec3f::dot(wu, v), vec3f::dot(dv, w), vec3f::dot(wu, direction0));

        return true;
      }

      return false;
    }

    static void findMinMax(float a, float b, float c, float& min, float& max)
    {
      min = max = a;

      if(b < min) min = b;
      if(b > max) max = b;

      if(c < min) min = c;
      if(c > max) max = c;
    }

    static bool testAABBEdgeWithTriangle(vec3f v0, vec3f v1, vec3f v2, float boxhalfsize, vec3f e)
    {
      float p0, p1, p2;
      float vmin, vmax;

      p0 = vec3f::dot(v0, e);
      p1 = vec3f::dot(v1, e);
      p2 = vec3f::dot(v2, e);

      vmin = std::min(p0, std::min(p1, p2));
      vmax = std::max(p0, std::max(p1, p2));

      if(vmin > boxhalfsize || vmax < -boxhalfsize) return false;//no collision

      return true;
    }

    static bool testTriangleEdgeWithAABB(vec3f t0, vec3f t1, vec3f t2, vec3f boxhalfsize, vec3f e)
    {
      //Project the AABB onto the triangle edge
      vec3f v0 = vec3f(-boxhalfsize[0], -boxhalfsize[1], -boxhalfsize[2]);
      vec3f v1 = vec3f(boxhalfsize[0], -boxhalfsize[1], -boxhalfsize[2]);
      vec3f v2 = vec3f(-boxhalfsize[0], -boxhalfsize[1], boxhalfsize[2]);
      vec3f v3 = vec3f(boxhalfsize[0], -boxhalfsize[1], boxhalfsize[2]);

      vec3f v4 = vec3f(-boxhalfsize[0], boxhalfsize[1], -boxhalfsize[2]);
      vec3f v5 = vec3f(boxhalfsize[0], boxhalfsize[1], -boxhalfsize[2]);
      vec3f v6 = vec3f(-boxhalfsize[0], boxhalfsize[1], boxhalfsize[2]);
      vec3f v7 = vec3f(boxhalfsize[0], boxhalfsize[1], boxhalfsize[2]);

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

    bool triangleCenteredBoxTest(vec3f v0, vec3f v1, vec3f v2, vec3f normal, vec3f boxhalfsize)
    {
      vec3f e0, e1, e2;

      e0 = v1 - v0;
      e1 = v2 - v1;
      e2 = v0 - v2;

      float vmin = UINT_MAX, vmax = INT_MIN;

      findMinMax(v0[0], v1[0], v2[0], vmin, vmax);
      if(vmin > boxhalfsize[0] || vmax < -boxhalfsize[0]) return false;

      findMinMax(v0[1], v1[1], v2[1], vmin, vmax);
      if(vmin > boxhalfsize[1] || vmax < -boxhalfsize[1]) return false;

      findMinMax(v0[2], v1[2], v2[2], vmin, vmax);
      if(vmin > boxhalfsize[2] || vmax < -boxhalfsize[2]) return false;

      if(!(testAABBEdgeWithTriangle(v0, v1, v2, boxhalfsize[0], vec3f(1, 0, 0)) &&
        testAABBEdgeWithTriangle(v0, v1, v2, boxhalfsize[1], vec3f(0, 1, 0)) &&
        testAABBEdgeWithTriangle(v0, v1, v2, boxhalfsize[2], vec3f(0, 0, 1))))
      {
        return false;
      }

      vec3f en0 = math::cross(normal, e0).normalize();
      vec3f en1 = math::cross(normal, e1).normalize();
      vec3f en2 = math::cross(normal, e2).normalize();

      if(!(testTriangleEdgeWithAABB(v0, v1, v2, boxhalfsize, en0) &&
        testTriangleEdgeWithAABB(v0, v1, v2, boxhalfsize, en1) &&
        testTriangleEdgeWithAABB(v0, v1, v2, boxhalfsize, en2)))
      {
        return false;
      }

      return true;// box and triangle overlaps
    }
  }
}