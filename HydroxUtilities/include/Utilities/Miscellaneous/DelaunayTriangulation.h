#ifndef DELAUNAYTRIANGULATION_H_
#define DELAUNAYTRIANGULATION_H_

#include <vector>
#include <list>
#include <map>

#include "Utilities/Math/Math.hpp"
#include "Utilities/Miscellaneous/CacheGenerator.h"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    struct HalfEdge
    {
      unsigned int face;//the face
      unsigned int twin;//the other edge in the other direction
      unsigned int next;//next half edge of the vertex in counter clockwise order
      unsigned int prev;//previous half edge of the vertex in counter clockwise order
      unsigned int vertex;//the origin of the half edge
    };

    class GRAPHICAPI DelaunayTriangulation
    {
    public:

      void generateTriangulation(const std::vector<Cache>& caches, std::vector<Cache>& outVoronoiSites);
      
    private:

      void circumcircle(vec3f a, vec3f b, vec3f c, vec3f& outCenter, float& outRadius);
      void divide(vec3f bbMin, vec3f bbMax, const std::vector<Cache>& caches, std::vector<Cache>& outVoronoiSites, std::vector<unsigned int>& outConvexHull);
      void merge(const std::vector<Cache>& voronoiSitesA, const std::vector<unsigned int>& convexHullA, std::vector<Cache>& voronoiSitesB, std::vector<unsigned int>& convexHullB);
      void calculateHorizon(std::vector<Cache>& voronoiSitesA, std::vector<unsigned int>& convexHullA, std::vector<Cache>& voronoiSitesB, std::vector<unsigned int>& convexHullB, std::list<unsigned int>& outHorizonA, std::list<unsigned int>& outHorizonB);
      void createRandomTetrahedron(std::vector<Cache>& caches, std::vector<Cache>& outVoronoiSites, std::vector<unsigned int>& outConvexHull);
      void constructDECL(const std::vector<unsigned int>& triangleIndices, std::vector<HalfEdge>& decl);
    };
  }
}

#endif