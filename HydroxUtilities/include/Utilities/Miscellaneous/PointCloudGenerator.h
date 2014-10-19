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
    class GRAPHICAPI PointCloudGenerator
    {
    public:

      struct Cache
      {
        vec3f normal;
        vec3f position;
      };

      struct PolygonData
      {
        vec3f centroid;
        vec2f normal;
        float area;
      };

      std::vector<std::list<Cache>> generateCaches(float maxDistance, float maxAngle, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices = std::vector<unsigned int>());
      void generateCachesPerVoxel(std::list<PolygonData> polygons, std::vector<std::vector<float>> normalBin, float maxAngle, std::list<Cache>& outCaches);

      float calculatePolygonAreaCentroid(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints, vec3f& outPolygonCentroid);

    private:

      void generateAABB(const std::vector<vec3f>& positions);

      inline float calculateTriangleArea(vec3f v0, vec3f v1, vec3f v2)
      {
        vec3f e0 = v1 - v0;
        vec3f e1 = v2 - v0;

        return math::cross(e0, e1).length() * 0.5f;
      }

      inline vec2f binToCylinder(unsigned int i, unsigned int thetaQ, unsigned int j, unsigned int uQ)
      {
        vec2f angle;
        angle[0] = he::util::math::degToRad(i * thetaQ) - he::util::math::PI;
        angle[1] = (j / uQ) * 2.0f - 1.0f;

        return angle;
      }

      inline vec2ui normalToBin(vec3f normal, unsigned int thetaQ, unsigned int uQ)
      {
        float theta = atan2f(normal[1], normal[0]);//cylinder coordinates (Spherical Sampling by Archimedes' Theorem)
        float u = normal[2];

        vec2ui bin;
        bin[0] = math::radToDeg(theta + math::PI) / thetaQ;
        bin[1] = (u * 0.5f + 0.5f) * uQ;

        return bin;
      }

      //float calculatePolygonArea(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints);//first vector contains the three points of the triangle to make all the rays

      vec3f m_bbMin, m_bbMax;

      std::vector<std::list<vec3f>> m_triangles;//saves all triangles for each voxel, 3 x xyz per triangle in a list
      std::vector<std::list<Cache>> m_caches;//saves all caches per voxel
    };
  }
}

#endif