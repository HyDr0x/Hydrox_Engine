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

    static void findMinMax(float a, float b, float c, float& min, float& max);
    static bool testAABBEdgeWithTriangle(vec3f v0, vec3f v1, vec3f v2, float boxhalfsize, vec3f e);
    static bool testTriangleEdgeWithAABB(vec3f t0, vec3f t1, vec3f t2, vec3f boxhalfsize, vec3f e);
    GRAPHICAPI bool triangleCenteredBoxTest(vec3f v0, vec3f v1, vec3f v2, vec3f normal, vec3f boxhalfsize);
  }
}

#endif