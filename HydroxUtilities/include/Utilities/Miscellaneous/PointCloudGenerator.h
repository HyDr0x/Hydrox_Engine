#ifndef POINTCLOUDGENERATOR_H_
#define POINTCLOUDGENERATOR_H_

#include <vector>
#include <list>
#include <map>

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

    typedef util::vec4us cacheIndexType;

    class GRAPHICAPI PointCloudGenerator
    {
    public:

      struct PolygonData
      {
        vec3f triangleVertices[3];
        vec3f centroid;
        vec3f normal;//normalized normal
        float area;
      };

      PointCloudGenerator() : m_epsilon(0.00001f)
      {
      }

      void generateCaches(std::vector<Cache>& outCaches, std::vector<vec2ui>& outTriangleCacheIndices, float errorRate, float maxDistance, float maxAngle, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices = std::vector<unsigned int>());
      
    private:

      inline vec2f binToCylinder(vec2ui bin, unsigned int thetaQ, unsigned int uQ)
      {
        vec2f angle;
        angle[0] = he::util::math::degToRad(float(bin[0] * thetaQ)) - he::util::math::PI;
        angle[1] = (float(bin[1]) / float(uQ)) * 2.0f - 1.0f;

        return angle;
      }

      inline vec2ui normalToBin(vec3f normal, unsigned int thetaQ, unsigned int uQ)
      {
        float theta = atan2f(normal[1], normal[0]);//cylinder coordinates (Spherical Sampling by Archimedes' Theorem)
        float u = normal[2];

        vec2ui bin;
        bin[0] = unsigned int(math::radToDeg(theta + math::PI)) / thetaQ;
        bin[1] = unsigned int(u * 0.5f + 0.5f * uQ);

        return bin;
      }

      inline float calculateTriangleArea(vec3f v0, vec3f v1, vec3f v2)
      {
        vec3f e0 = v1 - v0;
        vec3f e1 = v2 - v0;

        return float(math::cross(e0, e1).length()) * 0.5f;
      }

      void recursiveGenerateCaches(vec3f bbMin, vec3f bbMax, const std::vector<vec3f>& positions);
      void createCaches(vec3f bbMin, vec3f bbMax, const std::vector<vec3f>& positions);
      float calculatePolygonAreaCentroid(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints, vec3f& outPolygonCentroid);//first vector contains the three points of the triangle to make all the rays
      void generateCachesPerVoxel(unsigned int voxelIndex, std::vector<std::vector<float>> normalBin);
      void shiftCentroid(unsigned int voxelIndex);
      void reduceCaches();
      void sortCachesPerTriangle(const std::vector<vec3f>& positions);
      
      std::vector<std::vector<vec3f>> m_triangles;//saves all triangles for each voxel, 3 x xyz per triangle in a list
      std::vector<std::list<PolygonData>> m_polygons;//saves all polygons which lying in the voxel
      std::vector<std::list<PolygonData>> m_areaCaches;//saves all caches with their area
      std::map<unsigned int, std::list<PolygonData>> m_linearizedAreaCaches;
      std::vector<PolygonData> m_reducedCaches;
      std::vector<Cache> m_caches;
      std::vector<vec2ui> m_triangleCacheIndices;

      const float m_epsilon;
      vec3f m_globalBBMin, m_globalBBMax;
      float m_errorRate;
      float m_maxDistance;
      float m_maxAngle;
      unsigned int m_voxelNumber;
    };
  }
}

#endif