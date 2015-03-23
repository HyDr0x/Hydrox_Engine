#include "Utilities/Miscellaneous/VoronoiGenerator.h"

#include <cassert>
#include <random>
#include <cmath>

#include <voro++.hh>

#include "Utilities/Miscellaneous/SutherlandHodgman.h"

#include "Utilities/Math/Plane.h"

#include "Utilities/Miscellaneous/clipper.hpp"

namespace he
{
  namespace util
  {
    void VoronoiGenerator::initialize(float errorRate, float maxDistance, float maxAngle)
    {
    }

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

    void convertPolygonFloatPointsToIntegerPoints(const int bitShiftNumber, const Polygon& inPolygon, vec3f sideLength, ClipperLib::Path& outPolygon)
    {
      float maximum = std::max(sideLength[0], std::max(sideLength[1], sideLength[2]));

      float scale = std::pow(2.0f, bitShiftNumber) / maximum;//31 because we could multiply two 64 bit numbers, so the number could reach a * b = 2^31 * 2^31 = 2^62 and we have only 63 bits, so an exponent of 32 would lead to an overflow
      
      std::vector<vec3f> rotatedPoints = inPolygon.getRotatedPolygonPoints(vec3f(0, 0, 1));

      outPolygon.resize(inPolygon.getPointNumber());
      for(unsigned int i = 0; i < outPolygon.size(); i++)
      {
        outPolygon[i].X = rotatedPoints[i][0] * scale;
        outPolygon[i].Y = rotatedPoints[i][1] * scale;
      }
    }

    void VoronoiGenerator::generateCacheArea(const std::vector<vec3f>& triangles, std::vector<CacheGenerator::RawCache>& caches, vec3f bbMin, vec3f bbMax)
    {
      vec3f sideLength = bbMax - bbMin;

      for(unsigned int i = 0; i < 3; i++)
      {
        if(sideLength[i] < 0.0001f)
        {
          bbMin[i] -= 0.5f;
          bbMax[i] += 0.5f;
        }

        bbMin[i] -= sideLength[i] * 0.1f;//shift the outer limits of the bounding box so that no triangle can touch the border (numerical problems can occur with the sutherland hodgman algorithm otherwise)
        bbMax[i] += sideLength[i] * 0.1f;
      }

      int numberOfBlocks = ceil(pow(caches.size() / 5, 1.0f / 3.0f));

      voro::container container(bbMin[0], bbMax[0], bbMin[1], bbMax[1], bbMin[2], bbMax[2], numberOfBlocks, numberOfBlocks, numberOfBlocks, false, false, false, 8);

      for(unsigned int i = 0; i < caches.size(); i++)
      {
        container.put(i, caches[i].position[0], caches[i].position[1], caches[i].position[2]);
      }
      
      voro::c_loop_all voronoiLoop(container);

      if(voronoiLoop.start())
      {
        do
        {
          voro::voronoicell vc;

          container.compute_cell(vc, voronoiLoop);

          CacheGenerator::RawCache& cache = caches[voronoiLoop.pid()];

          std::vector<double> vertices;
          vc.vertices(cache.position[0], cache.position[1], cache.position[2], vertices);

          std::vector<int> faceVertices;
          vc.face_vertices(faceVertices);

          Plane plane(cache.triangleVertices[0], cache.triangleVertices[1], cache.triangleVertices[2]);

          std::vector<Polygon> polygons;

          //std::vector<double> normals;
          //vc.normals(normals);

          for(unsigned int i = 0; i < triangles.size(); i += 3)
          {
            std::vector<vec3f> trianglePoints(3);
            trianglePoints[0] = plane.projectPointOn(triangles[i]);
            trianglePoints[1] = plane.projectPointOn(triangles[i + 1]);
            trianglePoints[2] = plane.projectPointOn(triangles[i + 2]);

            Polygon polygon0(trianglePoints);
            Polygon polygon1;

            if(polygon0.getArea() < 0.0)
            {
              polygon0.invertWindingOrder();//winding order can be changed through plane projection
            }

            for(unsigned int j = 0; j < faceVertices.size(); j += faceVertices[j] + 1)
            {
            /*for(unsigned int j = 0; j < normals.size(); j += 3)
            {
              vec3f normal(normals[j], normals[j + 1], normals[j + 2]);
              vec3f binormal = math::cross(normal, vec3f(1, 2, 3));

              vec3f v0, v1, v2;

              v0 = cache.position + normal;
              v1 = v0 + binormal.normalize();
              v2 = v0 + math::cross(normal, binormal).normalize();*/

              vec3f v0, v1, v2;
              v0 = vec3f(vertices[3 * faceVertices[j + 1]], vertices[3 * faceVertices[j + 1] + 1], vertices[3 * faceVertices[j + 1] + 2]);
              v1 = vec3f(vertices[3 * faceVertices[j + 3]], vertices[3 * faceVertices[j + 3] + 1], vertices[3 * faceVertices[j + 3] + 2]);
              v2 = vec3f(vertices[3 * faceVertices[j + 2]], vertices[3 * faceVertices[j + 2] + 1], vertices[3 * faceVertices[j + 2] + 2]);

              Plane clippingPlane(v0, v1, v2);
              sutherlandHodgman(clippingPlane, polygon0, polygon1);

              polygon0 = polygon1;
              polygon1.clear();

              if(polygon0.getPointNumber() < 3)//we need polygons with an area, stop if it degenerates to a line or point or nothing
              {
                break;
              }
            }

            if(polygon0.getPointNumber() > 2)//take only triangles or more with an area
            {
              polygons.push_back(polygon0);
            }
          }

          ClipperLib::Paths areaPolygons;
          for(unsigned int i = 0; i < polygons.size(); i++)
          {
            ClipperLib::Path areaPolygon;
            convertPolygonFloatPointsToIntegerPoints(m_bitShiftNumber, polygons[i], sideLength, areaPolygon);
            areaPolygons.push_back(areaPolygon);
          }

          ClipperLib::Clipper clipper;
          ClipperLib::Paths unionPolygons;
          bool united;

          while(!areaPolygons.empty())
          {
            ClipperLib::Paths unionPolygon;
            unionPolygon.push_back(*areaPolygons.begin());
            areaPolygons.erase(areaPolygons.begin());

            do
            {
              united = false;
              for(ClipperLib::Paths::iterator it = areaPolygons.begin(); it != areaPolygons.end(); it++)
              {
                clipper.Clear();
                clipper.AddPaths(unionPolygon, ClipperLib::ptSubject, true);
                clipper.AddPath(*it, ClipperLib::ptClip, true);
                if(clipper.Execute(ClipperLib::ctUnion, unionPolygon))
                {
                  areaPolygons.erase(it);
                  united = true;
                  break;
                }
              }
            } while(united);

            unionPolygons.insert(unionPolygons.end(), unionPolygon.begin(), unionPolygon.end());
          }

          float maximum = std::max(sideLength[0], std::max(sideLength[1], sideLength[2]));
          float scale = 1.0f / (std::pow(2.0f, m_bitShiftNumber) / maximum);

          cache.area = 0.0f;
          for(unsigned int i = 0; i < unionPolygons.size(); i++)
          {
            cache.area += ClipperLib::Area(unionPolygons[i]);
          }

          cache.area *= scale;//clipper lib uses CW winding order?!
          cache.area *= scale;

          if(cache.area < 0.0f)
          {
            cache.area = 0.0f;
          }

          //if(cache.area > 100.0f)
          //{
          //  int ttrtg = 0.0f;
          //}

        } while(voronoiLoop.inc());
      }
    }
  }
}