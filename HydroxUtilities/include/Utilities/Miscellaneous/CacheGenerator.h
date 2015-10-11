#ifndef CACHEGENERATOR_H_
#define CACHEGENERATOR_H_

#include <vector>
#include <list>
#include <map>

#include "Utilities/Math/Math.hpp"
#include "Utilities/Math/Polygon.h"
#include "Utilities/Math/Triangle.h"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    struct Cache
    {
      vec3f position;
      float diffuseStrength;
      vec4f normal;//normalized normal [xyz], after the writing to the globalCacheBuffer normal.z becomes specularStrength and normal.w becomes specularExponent 
      vec4f area;//area which the cache represents | only x is used, the rest is padding
    };

    typedef util::vec4f cacheIndexType;

    class GRAPHICAPI CacheGenerator
    {
    public:

      void initialize(float errorRate, float maxDistance, float maxAngle, vec3f bbMin, vec3f bbMax);

      //OCTREE GENERATION SHOULD BE REPLACED BY UNIFIED TREE MODEL!!!
      void generateCaches(std::vector<Cache>& outCaches, std::vector<vec2ui>& outTriangleCacheIndices, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices = std::vector<unsigned int>());
      
      void generateCachesArea(const std::vector<vec3f>& triangles, vec3f bbMin, vec3f bbMax, std::vector<Cache>& outCaches, const std::vector<Triangle>& cacheTriangles);
      void generateCacheArea(const std::vector<vec3f>& triangles, vec3f bbMin, vec3f bbMax, std::vector<Cache>& caches, unsigned int cacheIndex, const Triangle& pickedTriangle);

      static vec3f findCacheTriangle(const std::vector<vec3f>& triangles, const Cache& cache, Triangle& cacheTriangle);//returns the nearest triangle of the cache and the nearest point on the triangle

      static void createTriangleCacheIndices(const std::vector<vec3f>& triangles, std::vector<Triangle>& cacheTriangles, std::vector<Cache>& caches, std::vector<vec2ui>& triangleCacheIndices);

      static void convertIndexedToNonIndexedTriangles(const std::vector<vec3f>& positions, const std::vector<unsigned int>& indices, std::vector<vec3f>& outTriangles);

    private:

      inline vec2f binToCylinder(vec2ui bin, unsigned int thetaQ, unsigned int phiQ)
      {
        vec2f angle;
        angle[0] = he::util::math::degToRad(float(bin[0] * thetaQ)) - he::util::math::PI;
        angle[1] = (float(bin[1]) / float(phiQ)) * 2.0f - 1.0f;

        return angle;
      }

      inline vec2ui normalToBin(vec3f normal, unsigned int thetaQ, unsigned int phiQ)
      {
        float theta = atan2f(normal[1], normal[0]);//cylinder coordinates (Spherical Sampling by Archimedes' Theorem)
        float phi = normal[2];

        vec2ui bin;
        bin[0] = unsigned int(math::radToDeg(theta + math::PI)) / thetaQ;
        bin[1] = unsigned int((phi * 0.5f + 0.5f) * phiQ);

        return bin;
      }

      void recursiveGenerateCaches(vec3f bbMin, vec3f bbMax, const std::vector<vec3f>& positions);
      void createCaches(vec3f bbMin, vec3f bbMax, const std::vector<vec3f>& positions);
      void generateCachesPerVoxel(unsigned int voxelIndex, std::vector<std::vector<float>> normalBin);
      void shiftCentroid(unsigned int voxelIndex);
      void reduceCaches();
      
      static const int m_bitShiftNumber = 31;

      std::vector<std::vector<vec3f>> m_triangles;//saves all triangles for each voxel, 3 x xyz per triangle in a list
      std::vector<std::list<std::pair<Polygon, Triangle>>> m_polygons;//saves all polygons which lying in the voxel and their original triangle from where they came
      std::map<unsigned int, std::list<std::pair<Cache, Triangle>>> m_linearizedAreaCaches;//saves all caches with their area
      std::vector<Cache> m_caches;
      std::vector<Triangle> m_cacheTriangles;
      std::vector<vec2ui> m_triangleCacheIndices;

      std::vector<vec3f> m_boxPoints;//create for each plane 3 points in the right orientation for the sutherland hodgman algorithm

      float m_epsilon;
      vec3f m_globalBBMin, m_globalBBMax;
      float m_errorRate;
      float m_maxDistance;
      float m_maxAngle;
      vec3ui m_voxelNumber;
    };
  }
}

#endif