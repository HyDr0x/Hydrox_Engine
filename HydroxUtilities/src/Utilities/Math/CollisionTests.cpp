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

    bool planeCenteredBoxCollision(vec3f planeNormal, vec3f v, vec3f boxHalfSize)
    {
      vec3f vmax, vmin;

      for(unsigned int i = 0; i < 3; i++)
      {
        if(planeNormal[i] > 0.0f)
        {
          vmin[i] = -boxHalfSize[i] - v[i];
          vmax[i] = boxHalfSize[i] - v[i];
        }
        else
        {
          vmin[i] = boxHalfSize[i] - v[i];
          vmax[i] = -boxHalfSize[i] - v[i];
        }
      }

      if(vec3f::dot(planeNormal, vmin) > 0.0f) return false;
      if(vec3f::dot(planeNormal, vmax) >= 0.0f) return true;

      return false;
    }

    float calculatePointTriangleDistance(vec3f t0, vec3f t1, vec3f t2, vec3f point, vec3f& nearestPoint)
    {
      vec3f triangleNormal = math::cross(t1 - t0, t2 - t0);
      vec3f pointPlanePos;

      linePlaneCollision(point, triangleNormal, t0, t1, t2, pointPlanePos);

      if(0.0f <= pointPlanePos[1] && pointPlanePos[1] <= 1.0f && 0.0f <= pointPlanePos[2] && pointPlanePos[2] <= 1.0f && pointPlanePos[1] + pointPlanePos[2] <= 1.0f)
      {
        vec3f direction = pointPlanePos[0] * triangleNormal;
        nearestPoint = point + direction;
        return direction.length();//point lies, projected on the triangle plane, in the triangle
      }
      else
      {
        vec3f e[3], tPoints0[3], tPoints1[3];
        tPoints0[0] = t0;
        tPoints0[1] = t0;
        tPoints0[2] = t1;

        tPoints1[0] = t1;
        tPoints1[1] = t2;
        tPoints1[2] = t2;

        float distance = UINT_MAX;
        float lineProj0, lineProj1, pointProj;
        float vmin, vmax;

        for(unsigned int i = 0; i < 3; i++)
        {
          e[i] = (tPoints1[i] - tPoints0[i]).normalize();

          lineProj0 = vec3f::dot(tPoints0[i] - tPoints0[i], e[i]);
          lineProj1 = vec3f::dot(tPoints1[i] - tPoints0[i], e[i]);
          pointProj = vec3f::dot(point - tPoints0[i], e[i]);

          vmin = std::min(lineProj0, lineProj1);
          vmax = std::max(lineProj0, lineProj1);

          if(vmin < pointProj && pointProj < vmax)
          {
            vec3f linePoint = tPoints0[i] + pointProj * e[i];
            float tmpDistance = (linePoint - point).length();
            if(distance > tmpDistance)
            {
              nearestPoint = linePoint;
              distance = tmpDistance;
            }
          }
          else if(pointProj < vmin)
          {
            if(lineProj0 == vmin)
            {
              float tmpDistance = (tPoints0[i] - point).length();
              if(distance > tmpDistance)
              {
                nearestPoint = tPoints0[i];
                distance = tmpDistance;
              }
            }
            else
            {
              float tmpDistance = (tPoints1[i] - point).length();
              if(distance > tmpDistance)
              {
                nearestPoint = tPoints1[i];
                distance = tmpDistance;
              }
            }
          }
          else
          {
            if(lineProj0 == vmax)
            {
              float tmpDistance = (tPoints0[i] - point).length();
              if(distance > tmpDistance)
              {
                nearestPoint = tPoints0[i];
                distance = tmpDistance;
              }
            }
            else
            {
              float tmpDistance = (tPoints1[i] - point).length();
              if(distance > tmpDistance)
              {
                nearestPoint = tPoints1[i];
                distance = tmpDistance;
              }
            }
          }
        }

        return distance;
      }
    }

    static void findMinMax(float a, float b, float c, float& min, float& max)
    {
      min = max = a;

      if(b < min) min = b;
      if(b > max) max = b;

      if(c < min) min = c;
      if(c > max) max = c;
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

    bool triangleCenteredBoxTest(vec3f v0, vec3f v1, vec3f v2, vec3f normal, vec3f boxHalfSize)
    {
      vec3f e0, e1, e2;

      e0 = v1 - v0;
      e1 = v2 - v1;
      e2 = v0 - v2;

      float vmin, vmax;

      findMinMax(v0[0], v1[0], v2[0], vmin, vmax);
      if(vmin > boxHalfSize[0] || vmax < -boxHalfSize[0]) return false;

      findMinMax(v0[1], v1[1], v2[1], vmin, vmax);
      if(vmin > boxHalfSize[1] || vmax < -boxHalfSize[1]) return false;

      findMinMax(v0[2], v1[2], v2[2], vmin, vmax);
      if(vmin > boxHalfSize[2] || vmax < -boxHalfSize[2]) return false;

      //if(!(testAABBEdgeWithTriangle(v0, v1, v2, boxHalfSize[0], vec3f(1, 0, 0)) &&
      //  testAABBEdgeWithTriangle(v0, v1, v2, boxHalfSize[1], vec3f(0, 1, 0)) &&
      //  testAABBEdgeWithTriangle(v0, v1, v2, boxHalfSize[2], vec3f(0, 0, 1)) &&
      //  testAABBEdgeWithTriangle(v0, v1, v2, boxHalfSize[0], vec3f(-1, 0, 0)) &&
      //  testAABBEdgeWithTriangle(v0, v1, v2, boxHalfSize[1], vec3f(0,-1, 0)) &&
      //  testAABBEdgeWithTriangle(v0, v1, v2, boxHalfSize[2], vec3f(0, 0,-1))))
      //{
      //  return false;
      //}

      vec3f en0 = math::cross(normal, e0).normalize();
      vec3f en1 = math::cross(normal, e1).normalize();
      vec3f en2 = math::cross(normal, e2).normalize();

      if(!(testTriangleEdgeWithAABB(v0, v1, v2, boxHalfSize, en0) &&
           testTriangleEdgeWithAABB(v0, v1, v2, boxHalfSize, en1) &&
           testTriangleEdgeWithAABB(v0, v1, v2, boxHalfSize, en2) &&
           testTriangleEdgeWithAABB(v0, v1, v2, boxHalfSize, normal.normalize())))
      {
        return false;
      }

      return true;// box and triangle overlaps
    }

    bool testXAxis(vec3f v0, vec3f v1, float a, float b, vec3f boxHalfSize)
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

    bool testYAxis(vec3f v0, vec3f v1, float a, float b, vec3f boxHalfSize)
    {
      float p0, p1, p2;

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

    bool testZAxis(vec3f v0, vec3f v1, float a, float b, vec3f boxHalfSize)
    {
      float p0, p1, p2;

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

    bool fastTriangleCenteredBoxTest(vec3f v0, vec3f v1, vec3f v2, vec3f normal, vec3f boxHalfSize)
    {
      vec3f e[3];

      e[0] = v1 - v0;
      e[1] = v2 - v1;
      e[2] = v0 - v2;

      float vmin, vmax;

      findMinMax(v0[0], v1[0], v2[0], vmin, vmax);
      if(vmin > boxHalfSize[0] || vmax < -boxHalfSize[0]) return false;

      findMinMax(v0[1], v1[1], v2[1], vmin, vmax);
      if(vmin > boxHalfSize[1] || vmax < -boxHalfSize[1]) return false;

      findMinMax(v0[2], v1[2], v2[2], vmin, vmax);
      if(vmin > boxHalfSize[2] || vmax < -boxHalfSize[2]) return false;

      if(!planeCenteredBoxCollision(normal, v0, boxHalfSize)) return false;

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