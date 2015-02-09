#ifndef SUTHERLANDHODGMAN_HPP_
#define SUTHERLANDHODGMAN_HPP_

#include <vector>

#include "Utilities/Math/Math.hpp"
#include "Utilities/Math/CollisionTests.h"

namespace he
{
  namespace util
  {
    void suhterlandHodgman(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints, std::vector<vec3f>& outPoints)
    {
      float epsilon = 0.00001f;
      outPoints = inPoints;

      vec3f output;

      for(unsigned int i = 0; i < 6; i++)//all 6 box planes
      {
        inPoints = outPoints;
        outPoints.clear();

        vec3f planeE0 = boxPoints[3 * i + 1] - boxPoints[3 * i];
        vec3f planeE1 = boxPoints[3 * i + 2] - boxPoints[3 * i];
        vec3f planeNormal = math::cross(planeE0, planeE1);

        vec3f s = inPoints[inPoints.size() - 1];
        for(unsigned int j = 0; j < inPoints.size(); j++)//all three triangle edges
        {
          vec3f e = inPoints[j];
          if(vec3f::dot(planeNormal, e - boxPoints[3 * i]) <= epsilon)
          {
            vec3f triangleEdge = s - e;
            if(linePlaneCollision(e, triangleEdge, boxPoints[3 * i], boxPoints[3 * i + 1], boxPoints[3 * i + 2], output) && epsilon < output[0] && output[0] + epsilon < 1.0f)
            {
              outPoints.push_back(e + output[0] * triangleEdge);
            }

            outPoints.push_back(e);
          }
          else if(vec3f::dot(planeNormal, s - boxPoints[3 * i]) <= epsilon)
          {
            vec3f triangleEdge = s - e;
            if(linePlaneCollision(e, triangleEdge, boxPoints[3 * i], boxPoints[3 * i + 1], boxPoints[3 * i + 2], output) && epsilon < output[0] && output[0] + epsilon < 1.0f)
            {
              outPoints.push_back(e + output[0] * triangleEdge);
            }
          }

          s = e;
        }

        if(outPoints.size() < 3)//triangle only touched the voxel
        {
          return;
        }
      }
    }
  }
}

#endif