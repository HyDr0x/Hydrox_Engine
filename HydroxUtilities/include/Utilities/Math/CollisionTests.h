#ifndef COLLISIONTESTS_HPP_
#define COLLISIONTESTS_HPP_

#include <math.h>

#include "math.hpp"
#include "Quaternion.hpp"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    GRAPHICAPI bool planePlaneCollison(vec3f supportVector0, vec3f normal0, vec3f supportVector1, vec3f normal1, vec3f& outPos, vec3f& outDirection);

    GRAPHICAPI bool linePlaneCollision(vec3f pos0, vec3f direction0, vec3f v0, vec3f v1, vec3f v2, vec3f& output);

    GRAPHICAPI bool planeCenteredBoxCollision(vec3f planeNormal, vec3f v, vec3f boxHalfSize);

    GRAPHICAPI float calculatePointTriangleDistance(vec3f t0, vec3f t1, vec3f t2, vec3f point, vec3f& nearestPoint);

    void findMinMax(float a, float b, float c, float& min, float& max);
    bool testAABBEdgeWithTriangle(vec3f v0, vec3f v1, vec3f v2, float boxHalfSize, vec3f e);
    bool testTriangleEdgeWithAABB(vec3f t0, vec3f t1, vec3f t2, vec3f boxHalfSize, vec3f e);
    GRAPHICAPI bool triangleCenteredBoxTest(vec3f v0, vec3f v1, vec3f v2, vec3f normal, vec3f boxHalfSize);
    GRAPHICAPI bool fastTriangleCenteredBoxTest(vec3f v0, vec3f v1, vec3f v2, vec3f normal, vec3f boxHalfSize);
  }
}

#endif