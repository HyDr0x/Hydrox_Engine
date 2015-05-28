#include "Utilities/Miscellaneous/CacheGenerator.h"

#include <iostream>
#include <cassert>
#include <random>
#include <cmath>

#include <voro++.hh>

#include "Utilities/Miscellaneous/clipper.hpp"
#include "Utilities/Miscellaneous/SutherlandHodgman.h"
#include "Utilities/Math/Triangle.h"

namespace he
{
  namespace util
  {
    void CacheGenerator::initialize(float errorRate, float maxDistance, float maxAngle, vec3f bbMin, vec3f bbMax)
    {
      m_globalBBMin = bbMin;
      m_globalBBMax = bbMax;

      m_errorRate = errorRate;
      m_maxDistance = maxDistance;
      m_maxAngle = maxAngle;

      float boxHalfSize = m_maxDistance * 0.5f;

      m_boxPoints.resize(18);//create for each plane 3 points in the right orientation for collision tests later
      m_boxPoints[0] = vec3f(-boxHalfSize, -boxHalfSize, -boxHalfSize);
      m_boxPoints[1] = vec3f(boxHalfSize, -boxHalfSize, -boxHalfSize);
      m_boxPoints[2] = vec3f(-boxHalfSize, -boxHalfSize, boxHalfSize);

      m_boxPoints[3] = vec3f(-boxHalfSize, -boxHalfSize, -boxHalfSize);
      m_boxPoints[4] = vec3f(-boxHalfSize, boxHalfSize, -boxHalfSize);
      m_boxPoints[5] = vec3f(boxHalfSize, -boxHalfSize, -boxHalfSize);

      m_boxPoints[6] = vec3f(-boxHalfSize, -boxHalfSize, -boxHalfSize);
      m_boxPoints[7] = vec3f(-boxHalfSize, -boxHalfSize, boxHalfSize);
      m_boxPoints[8] = vec3f(-boxHalfSize, boxHalfSize, -boxHalfSize);

      m_boxPoints[9] = vec3f(boxHalfSize, boxHalfSize, boxHalfSize);
      m_boxPoints[10] = vec3f(boxHalfSize, boxHalfSize, -boxHalfSize);
      m_boxPoints[11] = vec3f(-boxHalfSize, boxHalfSize, boxHalfSize);

      m_boxPoints[12] = vec3f(boxHalfSize, boxHalfSize, boxHalfSize);
      m_boxPoints[13] = vec3f(-boxHalfSize, boxHalfSize, boxHalfSize);
      m_boxPoints[14] = vec3f(boxHalfSize, -boxHalfSize, boxHalfSize);

      m_boxPoints[15] = vec3f(boxHalfSize, boxHalfSize, boxHalfSize);
      m_boxPoints[16] = vec3f(boxHalfSize, -boxHalfSize, boxHalfSize);
      m_boxPoints[17] = vec3f(boxHalfSize, boxHalfSize, -boxHalfSize);
    }

    void CacheGenerator::recursiveGenerateCaches(vec3f bbMin, vec3f bbMax, const std::vector<vec3f>& positions)
    {
      std::vector<vec3f> newPositions;

      vec3f boxSize = bbMax - bbMin;

      vec3f boxCenter(bbMin + boxSize * 0.5f);

      for(unsigned int i = 0; i < positions.size(); i += 3)
      {
        vec3f t0 = positions[i];
        vec3f t1 = positions[i + 1];
        vec3f t2 = positions[i + 2];

        Triangle triangle(t0, t1, t2);

        if(triangle.collisionCenteredBoxTest(boxCenter, boxSize * 0.5f, m_epsilon))
        {
          newPositions.push_back(t0);
          newPositions.push_back(t1);
          newPositions.push_back(t2);
        }
      }

      if(!newPositions.empty())
      {
        float sideMax = std::max(boxSize[0], std::max(boxSize[1], boxSize[2]));
        if(sideMax > m_maxDistance + m_epsilon)
        {
          vec3f newBoxSize;
          newBoxSize[0] = boxSize[0] > m_maxDistance + m_epsilon ? boxSize[0] * 0.5f : boxSize[0];
          newBoxSize[1] = boxSize[1] > m_maxDistance + m_epsilon ? boxSize[1] * 0.5f : boxSize[1];
          newBoxSize[2] = boxSize[2] > m_maxDistance + m_epsilon ? boxSize[2] * 0.5f : boxSize[2];

          for(unsigned int x = 0; x < 2; x++)
          {
            for(unsigned int y = 0; y < 2; y++)
            {
              for(unsigned int z = 0; z < 2; z++)
              {
                vec3f newbbMin = bbMin + vec3f(x * newBoxSize[0], y * newBoxSize[1], z * newBoxSize[2]);
                vec3f newbbMax = bbMin + vec3f((x + 1) * newBoxSize[0], (y + 1) * newBoxSize[1], (z + 1) * newBoxSize[2]);

                vec3f newBoxSize = newbbMax - newbbMin;
                float newSideMax = std::max(newBoxSize[0], std::max(newBoxSize[1], newBoxSize[2]));

                if(newbbMax <= bbMax + m_epsilon)
                {
                  recursiveGenerateCaches(newbbMin, newbbMax, newPositions);
                }
              }
            }
          }
        }
        else
        {
          createCaches(bbMin, bbMax, newPositions);
        }
      }
    }

    void CacheGenerator::createCaches(vec3f bbMin, vec3f bbMax, const std::vector<vec3f>& positions)
    {
      bbMin = math::vector_cast<float>(math::vector_cast<unsigned int>((bbMin - m_globalBBMin) / m_maxDistance + 0.5f)) * m_maxDistance + m_globalBBMin;
      bbMax = bbMin + m_maxDistance;
      float boxHalfSize = m_maxDistance * 0.5f;

      vec3f boxCenter(bbMin + vec3f(boxHalfSize, boxHalfSize, boxHalfSize));

      vec3ui voxelIndex3D(math::vector_cast<unsigned int>(math::abs<float>(boxCenter - m_globalBBMin) / m_maxDistance));
      voxelIndex3D = vec3ui(std::min(voxelIndex3D[0], m_voxelNumber[0] - 1), std::min(voxelIndex3D[1], m_voxelNumber[1] - 1), std::min(voxelIndex3D[2], m_voxelNumber[2] - 1));

      unsigned int voxelIndex = voxelIndex3D[0] * m_voxelNumber[1] * m_voxelNumber[2] + voxelIndex3D[1] * m_voxelNumber[2] + voxelIndex3D[2];

      std::vector<std::vector<float>> normalBin(18);//area of the triangles which have that normal

      for(unsigned int i = 0; i < normalBin.size(); i++)
      {
        normalBin[i].resize(normalBin.size(), 0.0f);
      }

      unsigned int  thetaQuantizer = 360 / normalBin[0].size();
      unsigned int  phiQuantizer = normalBin[0].size();

      for(unsigned int i = 0; i < positions.size(); i += 3)
      {
        vec3f t0 = positions[i];
        vec3f t1 = positions[i + 1];
        vec3f t2 = positions[i + 2];

        std::vector<vec3f> trianglePoints(3);
        trianglePoints[0] = t0 - boxCenter;
        trianglePoints[1] = t1 - boxCenter;
        trianglePoints[2] = t2 - boxCenter;

        vec3f normal = math::cross(trianglePoints[1] - trianglePoints[0], trianglePoints[2] - trianglePoints[0]).normalize();

        m_triangles[voxelIndex].push_back(t0);
        m_triangles[voxelIndex].push_back(t1);
        m_triangles[voxelIndex].push_back(t2);

        Polygon polygon = cutPolygonBox(trianglePoints, m_boxPoints, m_epsilon);//calculate area of the part of the triangle which lies in the voxel an return the centroid of that polygon

        if(polygon.getPointNumber() < 3 || math::floatSafeEqual(polygon.getArea(), 0.0f))//triangle only touched the voxel
        {
          continue;
        }

        for(unsigned int i = 0; i < polygon.getPointNumber(); i++)
        {
          polygon.setPoint(i, polygon[i] + boxCenter);
        }

        /////ASSERTION
        //assert(bbMin[0] - 0.001f <= data.centroid[0] && data.centroid[0] <= bbMax[0] + 0.001f && bbMin[1] - 0.001f <= data.centroid[1] && data.centroid[1] <= bbMax[1] + 0.001f && bbMin[2] - 0.001f <= data.centroid[2] && data.centroid[2] <= bbMax[2] + 0.001f);
        /////

        vec2ui bin = normalToBin(normal, thetaQuantizer, phiQuantizer);
        bin[0] = bin[0] % normalBin.size();
        bin[1] = bin[1] == normalBin.size() ? normalBin.size() - 1 : bin[1];
        normalBin[bin[0]][bin[1]] += polygon.getArea();
        m_polygons[voxelIndex].push_back(std::pair<Polygon, Triangle>(polygon, Triangle(t0, t1, t2)));
      }

      if(!m_polygons[voxelIndex].empty())
      {
        generateCachesPerVoxel(voxelIndex, normalBin);
        shiftCentroid(voxelIndex);
      }
    }

    void CacheGenerator::generateCachesPerVoxel(unsigned int voxelIndex, std::vector<std::vector<float>> normalBin)
    {
      //float epsilon = 0.001f;
      unsigned int  thetaQuantizer = 360 / normalBin[0].size();
      unsigned int  phiQuantizer = normalBin[0].size();

      unsigned int emptyBins = 1;

      std::list<std::pair<Polygon, Triangle>> polygons = m_polygons[voxelIndex];

      while(emptyBins && !polygons.empty())
      {
        emptyBins = normalBin.size() * normalBin.size();
        vec2ui maxBin;
        float maxArea = 0;

        for(unsigned int i = 0; i < normalBin.size(); i++)
        {
          for(unsigned int j = 0; j < normalBin[i].size(); j++)
          {
            if(normalBin[i][j] <= m_epsilon)
            {
              emptyBins--;
            }

            if(normalBin[i][j] > maxArea)
            {
              maxArea = normalBin[i][j];
              maxBin = vec2ui(i, j);
            }
          }
        }

        vec2f angle0 = binToCylinder(vec2ui(maxBin[0], maxBin[1] == normalBin.size() ? normalBin.size() - 1 : maxBin[1]), thetaQuantizer, phiQuantizer);
        vec2f angle1 = binToCylinder(vec2ui(maxBin[0] + 1, maxBin[1] + 1 > normalBin.size() ? normalBin.size() : maxBin[1] + 1), thetaQuantizer, phiQuantizer);

        float newTheta = (angle0[0] + angle1[0]) * 0.5f;
        float newU = (angle0[1] + angle1[1]) * 0.5f;

        he::util::vec3f qNormal;
        qNormal[0] = sqrt(1.0f - newU * newU) * cosf(newTheta);//calculate normal-bin
        qNormal[1] = sqrt(1.0f - newU * newU) * sinf(newTheta);
        qNormal[2] = newU;

        qNormal = qNormal.normalize();

        std::list<std::pair<Polygon, Triangle>> cachePolygons;
        std::list<std::pair<Polygon, Triangle>>::iterator pit = polygons.begin();
        while(pit != polygons.end())
        {
          if(acosf(math::clamp(vec3f::dot(pit->first.getNormal(), qNormal), -1.0f, 1.0f)) < m_maxAngle)//take the normal only if the angle to the normal-bin is not larger than 'maxAngle'
          {
            vec2ui bin = normalToBin(pit->first.getNormal(), thetaQuantizer, phiQuantizer);
            bin[0] = bin[0] % normalBin.size();
            bin[1] = bin[1] == normalBin.size() ? normalBin.size() - 1 : bin[1];
            normalBin[bin[0]][bin[1]] -= pit->first.getArea();
            cachePolygons.push_back(*pit);

            pit = polygons.erase(pit);

            if(polygons.empty()) break;
          }
          else
          {
            pit++;
          }
        }

        float summedArea = 0.0f;
        vec3f summedNormal = vec3f::identity();
        vec3f summedCentroid = vec3f::identity();

        for(std::list<std::pair<Polygon, Triangle>>::iterator it = cachePolygons.begin(); it != cachePolygons.end(); it++)
        {
          summedArea += it->first.getArea();
          summedCentroid += it->first.getCentroid() * it->first.getArea();
          summedNormal += it->first.getNormal() * it->first.getArea();
        }

        if(!cachePolygons.empty())
        {
          Cache cache;
          cache.position = summedCentroid / summedArea;
          cache.normal = (summedNormal / summedArea).normalize();
          cache.area = summedArea;

          /////ASSERTION
          //vec3ui voxelIndex3D(math::vector_cast<unsigned int>(math::abs<float>(cache.position - m_globalBBMin) / m_maxDistance));
          //assert(voxelIndex3D[0] * m_voxelNumber[1] * m_voxelNumber[2] + voxelIndex3D[1] * m_voxelNumber[2] + voxelIndex3D[2] == voxelIndex);
          /////

          m_linearizedAreaCaches[voxelIndex].push_back(std::pair<Cache, Triangle>(cache, Triangle(vec3f::identity(), vec3f::identity(), vec3f::identity())));
        }
        else
        {
          normalBin[maxBin[0]][maxBin[1]] = 0.0f;//set it to zero if we cant find a polygon
        }
      }

//#ifndef NDEBUG
      float lostArea = 0.0f;
      for(std::list<std::pair<Polygon, Triangle>>::iterator it = polygons.begin(); it != polygons.end(); it++)
      {
        lostArea += it->first.getArea();
      }

      if(lostArea > 0.0f)
      {
        std::clog << "Numeric Error:" << std::endl;
        std::clog << "Lost polygon area: " << lostArea << std::endl;
        std::clog << std::endl;
      }
//#endif
    }

    void CacheGenerator::shiftCentroid(unsigned int voxelIndex)
    {
      for(std::list<std::pair<Cache, Triangle>>::iterator cit = m_linearizedAreaCaches[voxelIndex].begin(); cit != m_linearizedAreaCaches[voxelIndex].end(); cit++)
      {
        vec3f nearestPoint;
        vec3f triangleVertices[3];
        float error = FLT_MAX;

        for(std::list<std::pair<Polygon, Triangle>>::iterator pit = m_polygons[voxelIndex].begin(); pit != m_polygons[voxelIndex].end(); pit++)
        {
          vec3f tmpNearestPoint;
          float distance, tmpError;

          distance = pit->first.calculatePointPolygonDistance(cit->first.position, tmpNearestPoint);

          tmpError = distance * (sqrt(1.0f - math::clamp(vec3f::dot(pit->first.getNormal(), cit->first.normal), 0.0f, 1.0f)) + 0.01f);//the distance would be senseless without normal penalty term

          if(error > tmpError)
          {
            error = tmpError;
            nearestPoint = tmpNearestPoint;
            triangleVertices[0] = pit->second[0];
            triangleVertices[1] = pit->second[1];
            triangleVertices[2] = pit->second[2];
          }
        }

        cit->first.position = nearestPoint;
        cit->second = Triangle(triangleVertices[0], triangleVertices[1], triangleVertices[2]);
      }
    }

    void CacheGenerator::reduceCaches()//could be improved, at the moment caches are being deleted immediatly even if there could be another more cache with a bigger area which could delete the cache which deleted other caches
    {
      for(std::map<unsigned int, std::list<std::pair<Cache, Triangle>>>::iterator mip = m_linearizedAreaCaches.begin(); mip != m_linearizedAreaCaches.end(); mip++)
      {
        unsigned int voxelIndex = mip->first;
        std::list<std::pair<Cache, Triangle>>::iterator cit = mip->second.begin();
        while(cit != mip->second.end())
        {
          vec3ui voxelIndex3D(math::vector_cast<unsigned int>(math::abs<float>(cit->first.position - m_globalBBMin) / m_maxDistance));

          for(int nx = -1; nx < 2; nx++)
          {
            for(int ny = -1; ny < 2; ny++)
            {
              for(int nz = -1; nz < 2; nz++)
              {
                if(0 <= voxelIndex3D[0] + nx && voxelIndex3D[0] + nx < m_voxelNumber[0] && 0 <= voxelIndex3D[1] + ny && voxelIndex3D[1] + ny < m_voxelNumber[1] && 0 <= voxelIndex3D[2] + nz && voxelIndex3D[2] + nz < m_voxelNumber[2])
                {
                  unsigned int neighborVoxelIndex = voxelIndex + nx * m_voxelNumber[1] * m_voxelNumber[2] + ny * m_voxelNumber[2] + nz;

                  if(neighborVoxelIndex == voxelIndex) continue;// there can't be similar caches in the same voxel

                  std::list<std::pair<Cache, Triangle>>::iterator vcit = m_linearizedAreaCaches[neighborVoxelIndex].begin();
                  while(vcit != m_linearizedAreaCaches[neighborVoxelIndex].end())
                  {
                    if(acosf(math::clamp(vec3f::dot(vcit->first.normal, cit->first.normal), -1.0f, 1.0f)) < m_maxAngle * m_errorRate && (vcit->first.position - cit->first.position).length() < m_maxDistance * m_errorRate &&
                      (vcit->first.area != cit->first.area || vcit->first.normal != cit->first.normal || vcit->first.position != cit->first.position) && //is it diffrent to the actual cache?
                      vcit->first.area < cit->first.area)//which one has the biggest area?
                    {
                      vcit = m_linearizedAreaCaches[neighborVoxelIndex].erase(vcit);//if we found the cache, remove it from the global iterating list

                      if(m_linearizedAreaCaches[neighborVoxelIndex].empty()) break;
                    }
                    else
                    {
                      vcit++;
                    }
                  }
                }
              }
            }
          }

          cit++;
        }
      }

      for(std::map<unsigned int, std::list<std::pair<Cache, Triangle>>>::iterator mip = m_linearizedAreaCaches.begin(); mip != m_linearizedAreaCaches.end(); mip++)
      {
        for(std::list<std::pair<Cache, Triangle>>::iterator cit = mip->second.begin(); cit != mip->second.end(); cit++)
        {
          m_caches.push_back(cit->first);
          m_cacheTriangles.push_back(cit->second);
        }
      }
    }

    vec3f CacheGenerator::findCacheTriangle(const std::vector<vec3f>& triangles, const Cache& cache, Triangle& cacheTriangle)
    {
      vec3f nearestPoint;
      float error = FLT_MAX;

      for(unsigned int i = 0; i < triangles.size(); i += 3)
      {
        vec3f tmpNearestPoint;
        float distance, tmpError;

        Triangle testTriangle(triangles[i], triangles[i + 1], triangles[i + 2]);

        distance = testTriangle.calculatePointTriangleDistance(cache.position, tmpNearestPoint);

        tmpError = distance * (sqrt(1.0f - math::clamp(vec3f::dot(testTriangle.getNormal(), cache.normal), 0.0f, 1.0f)) + 0.01f);//the distance would be senseless without normal penalty term

        if(error > tmpError)
        {
          error = tmpError;
          nearestPoint = tmpNearestPoint;
          cacheTriangle = testTriangle;
        }
      }

      return nearestPoint;
    }

    void CacheGenerator::createTriangleCacheIndices(const std::vector<vec3f>& triangles, std::vector<Triangle>& cacheTriangles, std::vector<Cache>& caches, std::vector<vec2ui>& triangleCacheIndices)
    {
      std::vector<Cache> sortedCaches;
      std::vector<Triangle> sortedCacheTriangles;

      for(unsigned int i = 0; i < triangles.size(); i += 3)
      {
        vec3f t0 = triangles[i];
        vec3f t1 = triangles[i + 1];
        vec3f t2 = triangles[i + 2];

        unsigned int startIndex = ~0;
        std::vector<Triangle>::iterator rit = cacheTriangles.begin();
        std::vector<Cache>::iterator cit = caches.begin();
        while(rit != cacheTriangles.end())
        {
          if((*rit)[0] == t0 && (*rit)[1] == t1 && (*rit)[2] == t2)
          {
            if(startIndex == ~0)
            {
              startIndex = sortedCaches.size();
            }

            sortedCaches.push_back(*cit);
            sortedCacheTriangles.push_back(*rit);

            cit = caches.erase(cit);
            rit = cacheTriangles.erase(rit);
            if(cacheTriangles.empty()) break;
          }
          else
          {
            rit++;
            cit++;
          }
        }

        triangleCacheIndices.push_back(vec2ui(startIndex, sortedCaches.size()));
      }

      caches = sortedCaches;
      cacheTriangles = sortedCacheTriangles;
    }

    void convertPolygonFloatPointsToIntegerPoints(const int bitShiftNumber, const Polygon& inPolygon, vec3f sideLength, ClipperLib::Path& outPolygon)
    {
      float maximum = std::max(sideLength[0], std::max(sideLength[1], sideLength[2]));

      double scale = std::pow(2.0f, bitShiftNumber) / maximum;//31 because we could multiply two 64 bit numbers, so the number could reach a * b = 2^31 * 2^31 = 2^62 and we have only 63 bits, so an exponent of 32 would lead to an overflow

      std::vector<vec3f> rotatedPoints = inPolygon.getRotatedPolygonPoints(vec3f(0, 0, 1));

      outPolygon.resize(inPolygon.getPointNumber());
      for(unsigned int i = 0; i < outPolygon.size(); i++)
      {
        outPolygon[i].X = rotatedPoints[i][0] * scale;
        outPolygon[i].Y = rotatedPoints[i][1] * scale;
      }
    }

    void calculateCacheArea(Cache& cache, Triangle cacheTriangle, voro::voronoicell_base& vc, vec3f sideLength, const std::vector<vec3f>& triangles, float epsilon, int bitShiftNumber)
    {
      std::vector<double> vertices;
      vc.vertices(cache.position[0], cache.position[1], cache.position[2], vertices);

      std::vector<int> faceVertices;
      vc.face_vertices(faceVertices);

      Plane plane(cacheTriangle[0], cacheTriangle[1], cacheTriangle[2]);

      std::vector<Polygon> polygons;

      for(unsigned int i = 0; i < triangles.size(); i += 3)
      {
        std::vector<vec3f> trianglePoints(3);
        trianglePoints[0] = plane.projectPointOn(triangles[i]);
        trianglePoints[1] = plane.projectPointOn(triangles[i + 1]);
        trianglePoints[2] = plane.projectPointOn(triangles[i + 2]);

        Polygon polygon0(trianglePoints);
        Polygon polygon1;

        if(polygon0.getArea() < 0.0f)
        {
          polygon0.invertWindingOrder();//winding order can be changed through plane projection
        }
        else if(polygon0.getArea() == 0.0f)//if it degenerates to a line, stop
        {
          continue;
        }

        for(unsigned int j = 0; j < faceVertices.size(); j += faceVertices[j] + 1)
        {
          vec3f v0, v1, v2;
          v0 = vec3f(vertices[3 * faceVertices[j + 1]], vertices[3 * faceVertices[j + 1] + 1], vertices[3 * faceVertices[j + 1] + 2]);
          v1 = vec3f(vertices[3 * faceVertices[j + 3]], vertices[3 * faceVertices[j + 3] + 1], vertices[3 * faceVertices[j + 3] + 2]);
          v2 = vec3f(vertices[3 * faceVertices[j + 2]], vertices[3 * faceVertices[j + 2] + 1], vertices[3 * faceVertices[j + 2] + 2]);

          Plane clippingPlane(v0, v1, v2);
          sutherlandHodgman(clippingPlane, polygon0, epsilon, polygon1);

          polygon0 = polygon1;
          polygon1.clear();

          if(polygon0.getPointNumber() < 3 || polygon0.getArea() == 0.0f)//we need polygons with an area, stop if it degenerates to a line or point or nothing
          {
            break;
          }
        }

        if(polygon0.getPointNumber() > 2 && polygon0.getArea() > 0.0f)//take only triangles or more with an area
        {
          polygons.push_back(polygon0);
        }
      }

      ClipperLib::Paths areaPolygons;
      for(unsigned int i = 0; i < polygons.size(); i++)
      {
        ClipperLib::Path areaPolygon;
        convertPolygonFloatPointsToIntegerPoints(bitShiftNumber, polygons[i], sideLength, areaPolygon);

        if(ClipperLib::Area(areaPolygon) > 0.0)//push it only in here if its still a polygon after conversion (rotation and quantization)
        {
          areaPolygons.push_back(areaPolygon);
        }
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
          assert(!unionPolygon.empty());
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
      double scale = 1.0f / (std::pow(2.0f, bitShiftNumber) / maximum);

      double addedArea = 0.0f;
      for(unsigned int i = 0; i < unionPolygons.size(); i++)
      {
        addedArea += ClipperLib::Area(unionPolygons[i]);
      }

      addedArea *= scale;//clipper lib uses CW winding order?!
      addedArea *= scale;

      cache.area = addedArea;

      if(cache.area < 0.0f)
      {
        cache.area = 0.0f;
      }
    }

    void CacheGenerator::generateCachesArea(const std::vector<vec3f>& triangles, vec3f bbMin, vec3f bbMax, std::vector<Cache>& outCaches, const std::vector<Triangle>& cacheTriangles)
    {
      vec3f diagonal = bbMax - bbMin;
      float epsilon = 0.0001f * std::max<float>(1.0f, fabs(std::min<float>(diagonal[0], std::min<float>(diagonal[1], diagonal[2]))));
      const int bitShiftNumber = 31;

      for(unsigned int i = 0; i < 3; i++)
      {
        if(diagonal[i] < epsilon)
        {
          bbMin[i] -= epsilon;
          bbMax[i] += epsilon;
        }

        bbMin[i] -= diagonal[i] * 0.1f;//shift the outer limits of the bounding box so that no triangle can touch the border (numerical problems can occur with the sutherland hodgman algorithm otherwise)
        bbMax[i] += diagonal[i] * 0.1f;
      }

      int numberOfBlocks = ceil(pow(float(outCaches.size()) / 5.0f, 1.0f / 3.0f));

      voro::container container(bbMin[0], bbMax[0], bbMin[1], bbMax[1], bbMin[2], bbMax[2], numberOfBlocks, numberOfBlocks, numberOfBlocks, false, false, false, 8);

      for(unsigned int i = 0; i < outCaches.size(); i++)
      {
        container.put(i, outCaches[i].position[0], outCaches[i].position[1], outCaches[i].position[2]);
      }

      voro::c_loop_all voronoiLoop(container);

      if(voronoiLoop.start())
      {
        do
        {
          voro::voronoicell vc;

          container.compute_cell(vc, voronoiLoop);

          calculateCacheArea(outCaches[voronoiLoop.pid()], cacheTriangles[voronoiLoop.pid()], vc, diagonal, triangles, epsilon, bitShiftNumber);

        } while(voronoiLoop.inc());
      }
    }

    void CacheGenerator::generateCacheArea(const std::vector<vec3f>& triangles, vec3f bbMin, vec3f bbMax, std::vector<Cache>& caches, unsigned int cacheIndex, const Triangle& pickedTriangle)
    {
      vec3f diagonal = bbMax - bbMin;
      float epsilon = 0.0001f * std::max<float>(1.0f, fabs(std::min<float>(diagonal[0], std::min<float>(diagonal[1], diagonal[2]))));
      const int bitShiftNumber = 31;

      for(unsigned int i = 0; i < 3; i++)
      {
        if(diagonal[i] < epsilon)
        {
          bbMin[i] -= epsilon;
          bbMax[i] += epsilon;
        }

        bbMin[i] -= epsilon * diagonal[i];//shift the outer limits of the bounding box so that no triangle can touch the border (numerical problems can occur with the sutherland hodgman algorithm otherwise)
        bbMax[i] += epsilon * diagonal[i];
      }

      int numberOfBlocks = ceil(pow(float(caches.size()) / 5.0f, 1.0f / 3.0f));

      voro::container container(bbMin[0], bbMax[0], bbMin[1], bbMax[1], bbMin[2], bbMax[2], numberOfBlocks, numberOfBlocks, numberOfBlocks, false, false, false, 8);

      for(unsigned int i = 0; i < caches.size(); i++)
      {
        container.put(i, caches[i].position[0], caches[i].position[1], caches[i].position[2]);
      }

      std::vector<int> neighborIndices;
      voro::c_loop_all voronoiLoop(container);

      if(voronoiLoop.start())
      {
        do
        {
          if(cacheIndex == voronoiLoop.pid())
          {
            voro::voronoicell_neighbor vc;
            container.compute_cell(vc, voronoiLoop);

            calculateCacheArea(caches[cacheIndex], pickedTriangle, vc, diagonal, triangles, epsilon, bitShiftNumber);

            vc.neighbors(neighborIndices);
            break;
          }
        } while(voronoiLoop.inc());
      }

      for(std::vector<int>::iterator it = neighborIndices.begin(); it != neighborIndices.end();)//take only indices which are grater zero, because negative indices represent IDs to the walls (or better, container boundaries)
      {
        if(*it < 0)
        {
          it = neighborIndices.erase(it);
        }
        else
        {
          it++;
        }
      }

      std::vector<Cache> neighborCaches;
      std::vector<Triangle> neighborCacheTriangles;

      for(unsigned int i = 0; i < neighborIndices.size(); i++)//collect the triangles where the neighboring caches are lying on
      {
        Cache& newCache = caches[neighborIndices[i]];
        Triangle newCacheTriangle(vec3f::identity(), vec3f::identity(), vec3f::identity());

        float distance = FLT_MAX;

        for(unsigned int j = 0; j < triangles.size(); j += 3)
        {
          Triangle testTriangle = Triangle(triangles[j], triangles[j + 1], triangles[j + 2]);
          vec3f output;
          testTriangle.collisionLine(newCache.position, testTriangle.getNormal(), output);
          if(-1.0f <= output[1] + output[2] && output[1] + output[2] <= 0.0f && -1.0f <= output[2] && output[2] <= 0.0f && -1.0f <= output[1] && output[1] <= 0.0f && output[0] < distance)
          {
            distance = output[0];
            newCacheTriangle = testTriangle;
          }
        }

        neighborCaches.push_back(newCache);
        neighborCacheTriangles.push_back(newCacheTriangle);
      }

      if(voronoiLoop.start())
      {
        do
        {
          unsigned int vid = voronoiLoop.pid();
          for(unsigned int i = 0; i < neighborIndices.size(); i++)
          {
            if(neighborIndices[i] == voronoiLoop.pid())
            {
              voro::voronoicell vc;
              container.compute_cell(vc, voronoiLoop);

              calculateCacheArea(neighborCaches[i], neighborCacheTriangles[i], vc, diagonal, triangles, epsilon, bitShiftNumber);
            }
          }
        } while(voronoiLoop.inc());
      }

      for(unsigned int i = 0; i < neighborIndices.size(); i++)
      {
        caches[neighborIndices[i]].area = neighborCaches[i].area;
      }
    }

    void CacheGenerator::generateCaches(std::vector<Cache>& outCaches, std::vector<vec2ui>& outTriangleCacheIndices, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices)
    {
      m_caches.clear();
      m_triangleCacheIndices.clear();

      vec3f boxSize(m_globalBBMax - m_globalBBMin);

      vec3f sideMax;
      sideMax[0] = m_maxDistance * pow(2.0f, std::max(ceil(log(boxSize[0] / m_maxDistance) / log(2.0f)), 0.0f));
      sideMax[1] = m_maxDistance * pow(2.0f, std::max(ceil(log(boxSize[1] / m_maxDistance) / log(2.0f)), 0.0f));
      sideMax[2] = m_maxDistance * pow(2.0f, std::max(ceil(log(boxSize[2] / m_maxDistance) / log(2.0f)), 0.0f));

      m_globalBBMax[0] = m_globalBBMin[0] + sideMax[0];
      m_globalBBMax[1] = m_globalBBMin[1] + sideMax[1];
      m_globalBBMax[2] = m_globalBBMin[2] + sideMax[2];

      vec3f diagonal = m_globalBBMax - m_globalBBMin;
      m_epsilon = 0.0001f * std::max<float>(1.0f, fabs(std::min<float>(diagonal[0], std::min<float>(diagonal[1], diagonal[2]))));

      m_voxelNumber[0] = static_cast<unsigned int>(ceil(sideMax[0] / m_maxDistance));
      m_voxelNumber[1] = static_cast<unsigned int>(ceil(sideMax[1] / m_maxDistance));
      m_voxelNumber[2] = static_cast<unsigned int>(ceil(sideMax[2] / m_maxDistance));

      m_polygons.resize(m_voxelNumber[0] * m_voxelNumber[1] * m_voxelNumber[2]);
      m_triangles.resize(m_voxelNumber[0] * m_voxelNumber[1] * m_voxelNumber[2]);

      std::vector<vec3f> trianglePositions;
      if(!indices.empty())
      {
        convertIndexedToNonIndexedTriangles(positions, indices, trianglePositions);
      }
      else
      {
        trianglePositions = positions;
      }

      recursiveGenerateCaches(m_globalBBMin, m_globalBBMax, trianglePositions);

      reduceCaches();

      createTriangleCacheIndices(trianglePositions, m_cacheTriangles, m_caches, m_triangleCacheIndices);

      m_triangles.clear();
      m_polygons.clear();
      m_linearizedAreaCaches.clear();

      generateCachesArea(trianglePositions, m_globalBBMin, m_globalBBMax, m_caches, m_cacheTriangles);

      outCaches = m_caches;

      outTriangleCacheIndices = m_triangleCacheIndices;
    }

    void CacheGenerator::convertIndexedToNonIndexedTriangles(const std::vector<vec3f>& positions, const std::vector<unsigned int>& indices, std::vector<vec3f>& outTriangles)
    {
      for(unsigned int i = 0; i < indices.size(); i += 3)
      {
        outTriangles.push_back(positions[indices[i]]);
        outTriangles.push_back(positions[indices[i + 1]]);
        outTriangles.push_back(positions[indices[i + 2]]);
      }
    }
  }
}