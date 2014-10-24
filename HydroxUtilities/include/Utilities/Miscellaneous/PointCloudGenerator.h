#ifndef POINTCLOUDGENERATOR_H_
#define POINTCLOUDGENERATOR_H_

#include <vector>
#include <list>

#include "Utilities/Math/Math.hpp"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    struct Cache
    {
      vec3f normal;//normalized normal
      vec3f position;
    };

    class GRAPHICAPI PointCloudGenerator
    {
    public:

      struct PolygonData
      {
        vec3f centroid;
        vec3f normal;//normalized normal
        float area;
      };

      static std::list<Cache> generateCaches(float errorRate, float maxDistance, float maxAngle, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices = std::vector<unsigned int>());

    private:

      static inline vec2f binToCylinder(vec2ui bin, unsigned int thetaQ, unsigned int uQ)
      {
        vec2f angle;
        angle[0] = he::util::math::degToRad(float(bin[0] * thetaQ)) - he::util::math::PI;
        angle[1] = (float(bin[1]) / float(uQ)) * 2.0f - 1.0f;

        return angle;
      }

      static inline vec2ui normalToBin(vec3f normal, unsigned int thetaQ, unsigned int uQ)
      {
        float theta = atan2f(normal[1], normal[0]);//cylinder coordinates (Spherical Sampling by Archimedes' Theorem)
        float u = normal[2];

        vec2ui bin;
        bin[0] = unsigned int(math::radToDeg(theta + math::PI)) / thetaQ;
        bin[1] = unsigned int(u * 0.5f + 0.5f * uQ);

        return bin;
      }

      static inline float calculateTriangleArea(vec3f v0, vec3f v1, vec3f v2)
      {
        vec3f e0 = v1 - v0;
        vec3f e1 = v2 - v0;

        return float(math::cross(e0, e1).length()) * 0.5f;
      }

      static void generateAABB(const std::vector<vec3f>& positions, vec3f& bbMin, vec3f& bbMax);
      static float calculatePolygonAreaCentroid(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints, vec3f& outPolygonCentroid);//first vector contains the three points of the triangle to make all the rays
      static void generateCachesPerVoxel(std::list<PolygonData>& polygons, std::vector<std::vector<float>> normalBin, float maxAngle);
      static void shiftCentroid(std::list<PolygonData>& caches, std::vector<vec3f>& triangles);
      static void reduceCaches(float errorRate, float maxAngle, float maxDistance, vec3i voxelNumber, std::vector<std::list<PolygonData>>& caches, std::list<Cache>& outCaches);
    };
  }
}

#endif