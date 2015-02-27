#include "Utilities/Miscellaneous/CacheGenerator.h"

#include <cassert>
#include <random>
#include <cmath>

#include "Utilities/Math/CollisionTests.h"
#include "Utilities/Miscellaneous/SutherlandHodgman.hpp"

namespace he
{
  namespace util
  {
    void generateAABB(const std::vector<vec3f>& positions, vec3f& bbMin, vec3f& bbMax)
    {
      bbMin = util::vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
      bbMax = util::vec3f(FLT_MIN, FLT_MIN, FLT_MIN);

      for(unsigned int i = 0; i < positions.size(); i++)
      {
        for(unsigned int j = 0; j < 3; j++)
        {
          if(bbMin[j] > positions[i][j])
          {
            bbMin[j] = positions[i][j];
          }

          if(bbMax[j] < positions[i][j])
          {
            bbMax[j] = positions[i][j];
          }
        }
      }
    }

    void createUnindexedTriangles(std::vector<vec3f>& outPositions, const std::vector<vec3f>& positions, const std::vector<unsigned int>& indices)
    {
      for(unsigned int i = 0; i < indices.size(); i += 3)
      {
        outPositions.push_back(positions[indices[i]]);
        outPositions.push_back(positions[indices[i + 1]]);
        outPositions.push_back(positions[indices[i + 2]]);
      }
    }

    void CacheGenerator::initialize(float errorRate, float maxDistance, float maxAngle)
    {
      m_epsilon = 0.0001f;

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

        vec3f v0(t0 - boxCenter), v1(t1 - boxCenter), v2(t2 - boxCenter);

        vec3f e0 = v1 - v0;
        vec3f e1 = v2 - v0;
        vec3f e2 = v2 - v1;

        vec3f normal = math::cross(e0, e1).normalize();

        if(triangleCenteredBoxTest(v0, v1, v2, normal, boxSize * 0.5f))
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

        vec3f polygonCentroid;
        std::vector<vec3f> polygonPoints;
        float area = calculatePolygonAreaCentroid(trianglePoints, polygonCentroid, polygonPoints);//calculate area of the part of the triangle which lies in the voxel an return the centroid of that polygon

        if(area == 0)//triangle only touched the voxel
        {
          continue;
        }

        for(unsigned int i = 0; i < polygonPoints.size(); i++)
        {
          polygonPoints[i] += boxCenter;
        }

        PolygonData data;
        data.polygonPoints = polygonPoints;
        data.triangleVertices[0] = t0;
        data.triangleVertices[1] = t1;
        data.triangleVertices[2] = t2;
        data.area = area;
        data.centroid = polygonCentroid + boxCenter;
        data.normal = normal;

        /////ASSERTION
        //assert(bbMin[0] - 0.001f <= data.centroid[0] && data.centroid[0] <= bbMax[0] + 0.001f && bbMin[1] - 0.001f <= data.centroid[1] && data.centroid[1] <= bbMax[1] + 0.001f && bbMin[2] - 0.001f <= data.centroid[2] && data.centroid[2] <= bbMax[2] + 0.001f);
        /////

        vec2ui bin = normalToBin(normal, thetaQuantizer, phiQuantizer);
        bin[0] = bin[0] % normalBin.size();
        bin[1] = bin[1] == normalBin.size() ? normalBin.size() - 1 : bin[1];
        normalBin[bin[0]][bin[1]] += area;
        m_polygons[voxelIndex].push_back(data);
      }

      if(!m_polygons[voxelIndex].empty())
      {
        generateCachesPerVoxel(voxelIndex, normalBin);
        shiftCentroid(voxelIndex);
      }
    }

    float CacheGenerator::calculatePolygonAreaCentroid(std::vector<vec3f> inPoints, vec3f& outPolygonCentroid, std::vector<vec3f>& outPoints)
    {   
      suhterlandHodgman(inPoints, m_boxPoints, outPoints);

      if(outPoints.size() < 3)//triangle only touched the voxel
      {
        return 0;
      }

      float area = 0.0f;
      vec3f pivotPoint(outPoints[0]), secondPoint(outPoints[1]);
      std::vector<float> triangelAreas;
      std::vector<vec3f> triangleCentroids;
      for(unsigned int i = 2; i < outPoints.size(); i++)//triangulation through fan triangulation
      {
        triangelAreas.push_back(calculateTriangleArea(pivotPoint, secondPoint, outPoints[i]));
        triangleCentroids.push_back((pivotPoint + secondPoint + outPoints[i]) / 3.0f);
        area += triangelAreas.back();
        secondPoint = outPoints[i];
      }

      assert(area > 0.0f);

      vec3f centroid = vec3f::identity();
      for(unsigned int i = 0; i < triangleCentroids.size(); i++)
      {
        centroid[0] += triangleCentroids[i][0] * triangelAreas[i];
        centroid[1] += triangleCentroids[i][1] * triangelAreas[i];
        centroid[2] += triangleCentroids[i][2] * triangelAreas[i];
      }

      centroid[0] /= area;
      centroid[1] /= area;
      centroid[2] /= area;

      outPolygonCentroid = centroid;

      return area;
    }

    void CacheGenerator::generateCachesPerVoxel(unsigned int voxelIndex, std::vector<std::vector<float>> normalBin)
    {
      float epsilon = 0.001f;
      unsigned int  thetaQuantizer = 360 / normalBin[0].size();
      unsigned int  phiQuantizer = normalBin[0].size();

      unsigned int emptyBins = 1;

      std::list<PolygonData> polygons = m_polygons[voxelIndex];

      while(emptyBins && !polygons.empty())
      {
        emptyBins = normalBin.size() * normalBin.size();
        vec2ui maxBin;
        float maxArea = 0;

        for(unsigned int i = 0; i < normalBin.size(); i++)
        {
          for(unsigned int j = 0; j < normalBin[i].size(); j++)
          {
            if(normalBin[i][j] <= epsilon)
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

        std::list<PolygonData> cachePolygons;
        std::list<PolygonData>::iterator pit = polygons.begin();
        while(pit != polygons.end())
        {
          if(acosf(math::clamp(vec3f::dot(pit->normal, qNormal), -1.0f, 1.0f)) < m_maxAngle)//take the normal only if the angle to the normal-bin is not larger than 'maxAngle'
          {
            vec2ui bin = normalToBin(pit->normal, thetaQuantizer, phiQuantizer);
            bin[0] = bin[0] % normalBin.size();
            bin[1] = bin[1] == normalBin.size() ? normalBin.size() - 1 : bin[1];
            normalBin[bin[0]][bin[1]] -= pit->area;
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

        for(std::list<PolygonData>::iterator it = cachePolygons.begin(); it != cachePolygons.end(); it++)
        {
          summedArea += it->area;
          summedCentroid += it->centroid * it->area;
          summedNormal += it->normal * it->area;
        }

        RawCache cache;
        cache.position = summedCentroid / summedArea;
        cache.normal = (summedNormal / summedArea).normalize();
        cache.area = summedArea;

        /////ASSERTION
        //vec3ui voxelIndex3D(math::vector_cast<unsigned int>(math::abs<float>(cache.position - m_globalBBMin) / m_maxDistance));
        //assert(voxelIndex3D[0] * m_voxelNumber[1] * m_voxelNumber[2] + voxelIndex3D[1] * m_voxelNumber[2] + voxelIndex3D[2] == voxelIndex);
        /////

        m_linearizedAreaCaches[voxelIndex].push_back(cache);
      }
    }

    void CacheGenerator::shiftCentroid(unsigned int voxelIndex)
    {
      for(std::list<RawCache>::iterator cit = m_linearizedAreaCaches[voxelIndex].begin(); cit != m_linearizedAreaCaches[voxelIndex].end(); cit++)
      {
        vec3f nearestPoint;
        vec3f triangleVertices[3];
        float error = FLT_MAX;

        for(std::list<PolygonData>::iterator pit = m_polygons[voxelIndex].begin(); pit != m_polygons[voxelIndex].end(); pit++)
        {
          vec3f tmpNearestPoint;
          float distance, tmpError;

          distance = calculatePointPolygonDistance(pit->polygonPoints, cit->position, tmpNearestPoint);

          tmpError = distance * (sqrt(1.0f - math::clamp(vec3f::dot(pit->normal, cit->normal), 0.0f, 1.0f)) + 0.01f);//the distance would be senseless without normal penalty term

          if(error > tmpError)
          {
            error = tmpError;
            nearestPoint = tmpNearestPoint;
            triangleVertices[0] = pit->triangleVertices[0];
            triangleVertices[1] = pit->triangleVertices[1];
            triangleVertices[2] = pit->triangleVertices[2];
          }
        }

        /////ASSERTION
        //vec3ui voxelIndex3D(math::vector_cast<unsigned int>(math::abs<float>(nearestPoint - m_globalBBMin) / m_maxDistance));
        //vec3ui NVoxelIndex3D(voxelIndex / (m_voxelNumber[1] * m_voxelNumber[2]), (voxelIndex % (m_voxelNumber[1] * m_voxelNumber[2])) / m_voxelNumber[2], voxelIndex % m_voxelNumber[2]);
        //assert(voxelIndex3D[0] * m_voxelNumber[1] * m_voxelNumber[2] + voxelIndex3D[1] * m_voxelNumber[2] + voxelIndex3D[2] == voxelIndex);
        /////

        cit->position = nearestPoint;
        cit->triangleVertices[0] = triangleVertices[0];
        cit->triangleVertices[1] = triangleVertices[1];
        cit->triangleVertices[2] = triangleVertices[2];
      }
    }

    void CacheGenerator::reduceCaches()//could be improved, at the moment caches are being deleted immediatly even if there could be another more cache with a bigger area which could delete the cache which deleted other caches
    {
      //unsigned int tmpSize = 0;
      //for(std::map<unsigned int, std::list<RawCache>>::iterator mip = m_linearizedAreaCaches.begin(); mip != m_linearizedAreaCaches.end(); mip++)
      //{
      //  tmpSize += mip->second.size();
      //}

      for(std::map<unsigned int, std::list<RawCache>>::iterator mip = m_linearizedAreaCaches.begin(); mip != m_linearizedAreaCaches.end(); mip++)
      {
        std::list<RawCache>::iterator cit = mip->second.begin();
        while(cit != mip->second.end())
        {
          vec3ui voxelIndex3D(math::vector_cast<unsigned int>(math::abs<float>(cit->position - m_globalBBMin) / m_maxDistance));
          unsigned int voxelIndex = voxelIndex3D[0] * m_voxelNumber[1] * m_voxelNumber[2] + voxelIndex3D[1] * m_voxelNumber[2] + voxelIndex3D[2];

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

                  std::list<RawCache>::iterator vcit = m_linearizedAreaCaches[neighborVoxelIndex].begin();
                  while(vcit != m_linearizedAreaCaches[neighborVoxelIndex].end())
                  {
                    if(acosf(math::clamp(vec3f::dot(vcit->normal, cit->normal), -1.0f, 1.0f)) < m_maxAngle * m_errorRate && (vcit->position - cit->position).length() < m_maxDistance * m_errorRate &&
                      (vcit->area != cit->area || vcit->normal != cit->normal || vcit->position != cit->position) && //is it diffrent to the actual cache?
                      vcit->area < cit->area)//which one has the biggest area?
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

      for(std::map<unsigned int, std::list<RawCache>>::iterator mip = m_linearizedAreaCaches.begin(); mip != m_linearizedAreaCaches.end(); mip++)
      {
        for(std::list<RawCache>::iterator cit = mip->second.begin(); cit != mip->second.end(); cit++)
        {
          m_reducedCaches.push_back(*cit);
        }
      }
    }

    void CacheGenerator::sortCachesPerTriangle(const std::vector<vec3f>& positions)
    {
      for(unsigned int i = 0; i < positions.size(); i += 3)
      {
        vec3f t0 = positions[i];
        vec3f t1 = positions[i + 1];
        vec3f t2 = positions[i + 2];

        unsigned int startIndex = ~0;
        std::vector<RawCache>::iterator rit = m_reducedCaches.begin();
        while(rit != m_reducedCaches.end())
        {
          if(math::abs(rit->triangleVertices[0] - t0) < vec3f(0.0001f, 0.0001f, 0.0001f) &&
            math::abs(rit->triangleVertices[1] - t1) < vec3f(0.0001f, 0.0001f, 0.0001f) &&
            math::abs(rit->triangleVertices[2] - t2) < vec3f(0.0001f, 0.0001f, 0.0001f))
          {
            if(startIndex == ~0)
            {
              startIndex = m_caches.size();
            }

            Cache cache;
            cache.position = rit->position;
            cache.normal = rit->normal;
            m_caches.push_back(cache);

            rit = m_reducedCaches.erase(rit);
            if(m_reducedCaches.empty()) break;
          }
          else
          {
            rit++;
          }
        }

        m_triangleCacheIndices.push_back(vec2ui(startIndex, m_caches.size()));
      }
    }

    void CacheGenerator::generateCaches(std::vector<Cache>& outCaches, std::vector<vec2ui>& outTriangleCacheIndices, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices)
    {
      m_caches.clear();
      m_triangleCacheIndices.clear();

      generateAABB(positions, m_globalBBMin, m_globalBBMax);

      vec3f boxSize(m_globalBBMax - m_globalBBMin);

      vec3f sideMax;
      sideMax[0] = m_maxDistance * pow(2.0f, std::max(ceil(log(boxSize[0] / m_maxDistance) / log(2.0f)), 0.0f));
      sideMax[1] = m_maxDistance * pow(2.0f, std::max(ceil(log(boxSize[1] / m_maxDistance) / log(2.0f)), 0.0f));
      sideMax[2] = m_maxDistance * pow(2.0f, std::max(ceil(log(boxSize[2] / m_maxDistance) / log(2.0f)), 0.0f));

      m_globalBBMax[0] = m_globalBBMin[0] + sideMax[0];
      m_globalBBMax[1] = m_globalBBMin[1] + sideMax[1];
      m_globalBBMax[2] = m_globalBBMin[2] + sideMax[2];

      m_voxelNumber[0] = static_cast<unsigned int>(ceil(sideMax[0] / m_maxDistance));
      m_voxelNumber[1] = static_cast<unsigned int>(ceil(sideMax[1] / m_maxDistance));
      m_voxelNumber[2] = static_cast<unsigned int>(ceil(sideMax[2] / m_maxDistance));

      m_polygons.resize(m_voxelNumber[0] * m_voxelNumber[1] * m_voxelNumber[2]);
      m_triangles.resize(m_voxelNumber[0] * m_voxelNumber[1] * m_voxelNumber[2]);

      std::vector<vec3f> trianglePositions;
      if(!indices.empty())
      {
        createUnindexedTriangles(trianglePositions, positions, indices);
      }
      else
      {
        trianglePositions = positions;
      }

      recursiveGenerateCaches(m_globalBBMin, m_globalBBMax, trianglePositions);

      reduceCaches();

      sortCachesPerTriangle(trianglePositions);

      m_triangles.clear();
      m_polygons.clear();
      m_linearizedAreaCaches.clear();

      outCaches = m_caches;
      outTriangleCacheIndices = m_triangleCacheIndices;
    }
  }
}