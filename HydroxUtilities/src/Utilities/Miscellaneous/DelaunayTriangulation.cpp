#include "Utilities/Miscellaneous/DelaunayTriangulation.h"

#include <cassert>
#include <random>
#include <cstdint>
#include <cmath>
#include <algorithm>

#include "Utilities/Math/CollisionTests.h"
#include "Utilities/Miscellaneous/SutherlandHodgman.hpp"

namespace he
{
  namespace util
  {
    void generateAABB(const std::vector<Cache>& caches, vec3f& bbMin, vec3f& bbMax)
    {
      bbMin = util::vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
      bbMax = util::vec3f(FLT_MIN, FLT_MIN, FLT_MIN);

      for(unsigned int i = 0; i < caches.size(); i++)
      {
        for(unsigned int j = 0; j < 3; j++)
        {
          if(bbMin[j] > caches[i].position[j])
          {
            bbMin[j] = caches[i].position[j];
          }

          if(bbMax[j] < caches[i].position[j])
          {
            bbMax[j] = caches[i].position[j];
          }
        }
      }
    }

    void circumcircle(vec3f a, vec3f b, vec3f c, vec3f& outCenter, float& outRadius)
    {
      a -= c;
      b -= c;

      float lA = a.length();
      float lB = b.length();

      float axb = math::cross(a, b).length();

      outRadius = (lA * lB * (a - b).length()) / (2.0f * axb);

      outCenter = math::cross((lA * lA) * b - (lB * lB) * a, math::cross(a, b)) / (2.0f * axb * axb) + c;
    }

    void DelaunayTriangulation::generateTriangulation(const std::vector<Cache>& caches, std::vector<Cache>& outVoronoiSites)
    {
      vec3f bbMin, bbMax;
      generateAABB(caches, bbMin, bbMax);

      
    }

    void DelaunayTriangulation::divide(vec3f bbMin, vec3f bbMax, const std::vector<Cache>& caches, std::vector<Cache>& outVoronoiSites, std::vector<unsigned int>& outConvexHull)
    {
      std::vector<Cache> newCaches;

      for(unsigned int i = 0; i < caches.size(); i++)
      {
        if(bbMin <= caches[i].position && caches[i].position <= bbMax)
        {
          newCaches.push_back(caches[i]);
        }
      }

      if(newCaches.size() > 4)//if we have more than four caches, divide it further
      {
        vec3f newBoxSize(0.5f * (bbMax - bbMin));

        bool createdTetrahedron = false;

        for(unsigned int x = 0; x < 2; x++)
        {
          for(unsigned int y = 0; y < 2; y++)
          {
            for(unsigned int z = 0; z < 2; z++)
            {
              vec3f newbbMin = bbMin + vec3f(x * newBoxSize[0], y * newBoxSize[1], z * newBoxSize[2]);
              vec3f newbbMax = bbMin + vec3f((x + 1) * newBoxSize[0], (y + 1) * newBoxSize[1], (z + 1) * newBoxSize[2]);

              std::vector<Cache> voronoiSites;
              std::vector<unsigned int> convexHull;
              divide(newbbMin, newbbMax, newCaches, voronoiSites, convexHull);
              merge(voronoiSites, convexHull, outVoronoiSites, outConvexHull);

              if(!outVoronoiSites.empty())
              {
                createdTetrahedron = true;
              }
            }
          }
        }

        if(!createdTetrahedron)
        {
          createRandomTetrahedron(newCaches, outVoronoiSites, outConvexHull);
          merge(newCaches, std::vector<unsigned int>(), outVoronoiSites, outConvexHull);
        }
      }
      else if(newCaches.size() <= 4)//if we have four or less caches, merge them to a point, line, triangle or tetrahedron
      {
        for(unsigned int i = 0; i < newCaches.size(); i++)
        {
          outVoronoiSites.push_back(newCaches[i]);
        }
      }
    }

    void DelaunayTriangulation::merge(const std::vector<Cache>& voronoiSitesA, const std::vector<unsigned int>& convexHullA, std::vector<Cache>& voronoiSitesB, std::vector<unsigned int>& convexHullB)
    {
      if(convexHullA.size() >= 4 && convexHullB.size() >= 4)
      {

      }
      else if(convexHullA.size() >= 4)
      {
      }
      else if(convexHullB.size() >= 4)
      {

      }
    }

    void DelaunayTriangulation::createRandomTetrahedron(std::vector<Cache>& caches, std::vector<Cache>& outVoronoiSites, std::vector<unsigned int>& outConvexHull)
    {
      std::vector<vec3f> trianglePoints(3);

      trianglePoints[0] = caches[0].position;
      trianglePoints[1] = caches[1].position;
      trianglePoints[2] = caches[2].position;

      vec3f e0 = trianglePoints[1] - trianglePoints[0];
      vec3f e1 = trianglePoints[2] - trianglePoints[0];
      vec3f normal = math::cross(e0, e1).normalize();

      float minLength = FLT_MAX;
      unsigned int minCache;

      for(unsigned int i = 0; i < caches.size(); i++)
      {
        vec3f nearestPoint;
        calculatePointPolygonDistance(trianglePoints, caches[i].position, nearestPoint);

        float length = (caches[i].position - nearestPoint).length();
        if(length < minLength)
        {
          minLength = length;
          minCache = i;
        }
      }

      outVoronoiSites.push_back(caches[0]);
      outVoronoiSites.push_back(caches[1]);
      outVoronoiSites.push_back(caches[2]);
      outVoronoiSites.push_back(caches[minCache]);

      caches.erase(caches.begin());
      caches.erase(caches.begin());
      caches.erase(caches.begin());

      unsigned int cacheIndex = 0;
      for(std::vector<Cache>::iterator it = caches.begin(); it != caches.end(); it++, cacheIndex++)
      {
        if(cacheIndex == minCache)
        {
          caches.erase(it);
        }
      }

      outConvexHull.push_back(0);
      outConvexHull.push_back(1);
      outConvexHull.push_back(2);

      outConvexHull.push_back(0);
      outConvexHull.push_back(1);
      outConvexHull.push_back(minCache);

      outConvexHull.push_back(2);
      outConvexHull.push_back(0);
      outConvexHull.push_back(minCache);

      outConvexHull.push_back(1);
      outConvexHull.push_back(2);
      outConvexHull.push_back(minCache);
    }

    void DelaunayTriangulation::constructDECL(const std::vector<unsigned int>& triangleIndices, std::vector<HalfEdge>& decl)
    {
      for(unsigned int i = 0; i < triangleIndices.size(); i += 3)
      {
        HalfEdge edge0;
        edge0.vertex = triangleIndices[i];
        edge0.twin = decl.size() + 1;
        edge0.face = i;
        edge0.prev = UINT32_MAX;
        edge0.next = UINT32_MAX;

        HalfEdge edge1;
        edge1.vertex = triangleIndices[i + 1];
        edge1.twin = decl.size();

        edge1.face = UINT32_MAX;
        for(unsigned int j = 0; j < triangleIndices.size(); j += 3)//find the opposite triangle
        {
          for(unsigned int k = 0; k < 3; k++)
          {
            unsigned int index0 = j + (k % 3), index1 = j + ((k + 1) % 3);
            if(!(triangleIndices[index0] == edge0.vertex && triangleIndices[index1] == edge1.vertex) && triangleIndices[index0] == edge1.vertex && triangleIndices[index1] == edge0.vertex)
            {
              edge1.face = j;
              break;
            }
          }

          if(edge1.face != UINT32_MAX)
          {
            break;
          }
        }

        edge1.prev = UINT32_MAX;
        edge1.next = UINT32_MAX;

        if(std::find(decl.begin(), decl.end(), edge0) == decl.end())
        {
          decl.push_back(edge0);
          decl.push_back(edge1);
        }
      }
    }
  }
}