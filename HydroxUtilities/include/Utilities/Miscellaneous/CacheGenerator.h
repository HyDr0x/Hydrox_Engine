#ifndef CACHEGENERATOR_H_
#define CACHEGENERATOR_H_

#include <vector>
#include <list>
#include <map>

#include "Utilities/Math/Math.hpp"
#include "Utilities/Math/Polygon.h"

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
      vec4f area;//only x is used, the rest is padding
    };

    typedef util::vec4f cacheIndexType;

    class GRAPHICAPI CacheGenerator
    {
    public:

      struct PolygonData
      {
        PolygonData(std::vector<vec3f> polygonPoints) : polygon(polygonPoints)
        {}

        PolygonData(const Polygon& otherPolygon) : polygon(otherPolygon)
        {}

        Polygon polygon;
        vec3f triangleVertices[3];//the triangle from which the polygon was created
      };

      struct RawCache
      {
        vec3f position;//position of the cache
        vec3f normal;//normalized normal
        float area;//area which the cache represents
        vec3f triangleVertices[3];//the triangle in which the cache lies
      };

      void initialize(float errorRate, float maxDistance, float maxAngle, vec3f bbMin, vec3f bbMax);

      //OCTREE GENERATION SHOULD BE REPLACED BY UNIFIED TREE MODEL!!!
      void generateCaches(std::vector<Cache>& outCaches, std::vector<vec2ui>& outTriangleCacheIndices, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices = std::vector<unsigned int>());
      
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
      Polygon calculatePolygon(std::vector<vec3f> inPoints);//first vector contains the three points of the triangle to make all the rays
      void generateCachesPerVoxel(unsigned int voxelIndex, std::vector<std::vector<float>> normalBin);
      void shiftCentroid(unsigned int voxelIndex);
      void reduceCaches();
      void sortCachesPerTriangle(const std::vector<vec3f>& positions);
      
      std::vector<std::vector<vec3f>> m_triangles;//saves all triangles for each voxel, 3 x xyz per triangle in a list
      std::vector<std::list<PolygonData>> m_polygons;//saves all polygons which lying in the voxel
      std::map<unsigned int, std::list<RawCache>> m_linearizedAreaCaches;//saves all caches with their area
      std::vector<RawCache> m_reducedCaches;
      std::vector<RawCache> m_caches;
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