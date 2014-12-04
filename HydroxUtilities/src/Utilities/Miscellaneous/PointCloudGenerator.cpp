#include "Utilities/Miscellaneous/PointCloudGenerator.h"

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

    void PointCloudGenerator::recursiveGenerateCaches(vec3f bbMin, vec3f bbMax, const std::vector<vec3f>& positions)
    {
      std::vector<vec3f> newPositions;

      float boxHalfSize = (bbMax[0] - bbMin[0]) * 0.5f;
      vec3f boxCenter(bbMin + (bbMax - bbMin) * 0.5f);

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

        if(triangleCenteredBoxTest(v0, v1, v2, normal, vec3f(boxHalfSize, boxHalfSize, boxHalfSize)))
        {
          newPositions.push_back(t0);
          newPositions.push_back(t1);
          newPositions.push_back(t2);
        }
      }

      if(!newPositions.empty())
      {
        if(2.0f * boxHalfSize > m_maxDistance + m_epsilon)
        {
          recursiveGenerateCaches(bbMin + vec3f(boxHalfSize, boxHalfSize, boxHalfSize), bbMax, newPositions);
          recursiveGenerateCaches(bbMin + vec3f(0, boxHalfSize, boxHalfSize), bbMax + vec3f(-boxHalfSize, 0, 0), newPositions);
          recursiveGenerateCaches(bbMin + vec3f(0, boxHalfSize, 0), bbMax + vec3f(-boxHalfSize, 0, -boxHalfSize), newPositions);
          recursiveGenerateCaches(bbMin + vec3f(boxHalfSize, boxHalfSize, 0), bbMax + vec3f(0, 0, -boxHalfSize), newPositions);

          recursiveGenerateCaches(bbMin + vec3f(boxHalfSize, 0, boxHalfSize), bbMax + vec3f(0, -boxHalfSize, 0), newPositions);
          recursiveGenerateCaches(bbMin + vec3f(0, 0, boxHalfSize), bbMax + vec3f(-boxHalfSize, -boxHalfSize, 0), newPositions);
          recursiveGenerateCaches(bbMin + vec3f(0, 0, 0), bbMax + vec3f(-boxHalfSize, -boxHalfSize, -boxHalfSize), newPositions);
          recursiveGenerateCaches(bbMin + vec3f(boxHalfSize, 0, 0), bbMax + vec3f(0, -boxHalfSize, -boxHalfSize), newPositions);
        }
        else
        {
          createCaches(bbMin, bbMax, newPositions);
        }
      }
    }

    void PointCloudGenerator::createCaches(vec3f bbMin, vec3f bbMax, const std::vector<vec3f>& positions)
    {
      float boxHalfSize = m_maxDistance * 0.5f;
      vec3f boxCenter(bbMin + vec3f(boxHalfSize, boxHalfSize, boxHalfSize));

      std::vector<vec3f> boxPoints(18);//create for each plane 3 points in the right orientation for collision tests later
      boxPoints[0] = vec3f(-boxHalfSize, -boxHalfSize, -boxHalfSize);
      boxPoints[1] = vec3f(boxHalfSize, -boxHalfSize, -boxHalfSize);
      boxPoints[2] = vec3f(-boxHalfSize, -boxHalfSize, boxHalfSize);

      boxPoints[3] = vec3f(-boxHalfSize, -boxHalfSize, -boxHalfSize);
      boxPoints[4] = vec3f(-boxHalfSize, boxHalfSize, -boxHalfSize);
      boxPoints[5] = vec3f(boxHalfSize, -boxHalfSize, -boxHalfSize);

      boxPoints[6] = vec3f(-boxHalfSize, -boxHalfSize, -boxHalfSize);
      boxPoints[7] = vec3f(-boxHalfSize, -boxHalfSize, boxHalfSize);
      boxPoints[8] = vec3f(-boxHalfSize, boxHalfSize, -boxHalfSize);

      boxPoints[9] = vec3f(boxHalfSize, boxHalfSize, boxHalfSize);
      boxPoints[10] = vec3f(boxHalfSize, boxHalfSize, -boxHalfSize);
      boxPoints[11] = vec3f(-boxHalfSize, boxHalfSize, boxHalfSize);

      boxPoints[12] = vec3f(boxHalfSize, boxHalfSize, boxHalfSize);
      boxPoints[13] = vec3f(-boxHalfSize, boxHalfSize, boxHalfSize);
      boxPoints[14] = vec3f(boxHalfSize, -boxHalfSize, boxHalfSize);

      boxPoints[15] = vec3f(boxHalfSize, boxHalfSize, boxHalfSize);
      boxPoints[16] = vec3f(boxHalfSize, -boxHalfSize, boxHalfSize);
      boxPoints[17] = vec3f(boxHalfSize, boxHalfSize, -boxHalfSize);

      vec3ui voxelIndex3D(math::vector_cast<unsigned int>(math::abs<float>(boxCenter - m_globalBBMin) / m_maxDistance));

      unsigned int voxelIndex = voxelIndex3D[0] * m_voxelNumber * m_voxelNumber + voxelIndex3D[1] * m_voxelNumber + voxelIndex3D[2];

      std::vector<std::vector<float>> normalBin(18);//area of the triangles which have that normal

      for(unsigned int i = 0; i < normalBin.size(); i++)
      {
        normalBin[i].resize(normalBin.size(), 0.0f);
      }

      unsigned int  thetaQuantizer = 360 / normalBin[0].size();
      unsigned int  uQuantizer = normalBin[0].size();

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

        if(triangleCenteredBoxTest(v0, v1, v2, normal, vec3f(boxHalfSize, boxHalfSize, boxHalfSize)))
        {
          m_triangles[voxelIndex].push_back(t0);
          m_triangles[voxelIndex].push_back(t1);
          m_triangles[voxelIndex].push_back(t2);

          std::vector<vec3f> trianglePoints(3);
          trianglePoints[0] = v0;
          trianglePoints[1] = v1;
          trianglePoints[2] = v2;

          vec3f polygonCentroid;

          float area = calculatePolygonAreaCentroid(trianglePoints, boxPoints, polygonCentroid);//calculate area of the part of the triangle which lies in the voxel an return the centroid of that polygon

          if(area == FLT_MAX)//triangle only touched the voxel
          {
            continue;
          }

          assert(area > 0.0f);

          normal = normal.normalize();

          PolygonData data;
          data.area = area;
          data.centroid = polygonCentroid + boxCenter;
          data.normal = normal;

          vec2ui bin = normalToBin(normal, thetaQuantizer, uQuantizer);
          bin[0] = bin[0] % normalBin.size();
          bin[1] = bin[1] == normalBin.size() ? normalBin.size() - 1 : bin[1];
          normalBin[bin[0]][bin[1]] += area;
          m_polygons[voxelIndex].push_back(data);
        }
      }

      if(!m_polygons[voxelIndex].empty())
      {
        generateCachesPerVoxel(voxelIndex, normalBin);
        shiftCentroid(voxelIndex);
        m_linearizedAreaCaches[voxelIndex].insert(m_linearizedAreaCaches[voxelIndex].end(), m_areaCaches[voxelIndex].begin(), m_areaCaches[voxelIndex].end());
      }
    }

    float PointCloudGenerator::calculatePolygonAreaCentroid(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints, vec3f& outPolygonCentroid)
    {
      std::vector<vec3f> outPoints = inPoints;

      vec3f output;
      //sutherland-hodgman algorithm
      for(unsigned int i = 0; i < 6; i++)//all 6 box planes
      {
        inPoints = outPoints;
        outPoints.clear();

        vec3f planeE0 = boxPoints[3 * i + 1] - boxPoints[3 * i];
        vec3f planeE1 = boxPoints[3 * i + 2] - boxPoints[3 * i];
        vec3f planeNormal = math::cross(planeE0, planeE1);

        vec3f s = inPoints[inPoints.size() - 1];
        for(unsigned int j = 0; j < inPoints.size(); j++)//all three triangle edges
        {
          vec3f e = inPoints[j];
          if(vec3f::dot(planeNormal, e - boxPoints[3 * i]) <= m_epsilon)
          {
            vec3f triangleEdge = s - e;
            if(linePlaneCollision(e, triangleEdge, boxPoints[3 * i], boxPoints[3 * i + 1], boxPoints[3 * i + 2], output) && m_epsilon < output[0] && output[0] + m_epsilon < 1.0f)
            {
              outPoints.push_back(e + output[0] * triangleEdge);
            }

            outPoints.push_back(e);
          }
          else if(vec3f::dot(planeNormal, s - boxPoints[3 * i]) <= m_epsilon)
          {
            vec3f triangleEdge = s - e;
            if(linePlaneCollision(e, triangleEdge, boxPoints[3 * i], boxPoints[3 * i + 1], boxPoints[3 * i + 2], output) && m_epsilon < output[0] && output[0] + m_epsilon < 1.0f)
            {
              outPoints.push_back(e + output[0] * triangleEdge);
            }
          }

          s = e;
        }
      }

      if(outPoints.size() < 3)//triangle only touched the voxel
      {
        return FLT_MAX;
      }

      float area = 0.0f;
      vec3f pivotPoint(outPoints[0]), secondPoint(outPoints[1]);
      for(unsigned int i = 2; i < outPoints.size(); i++)//triangulation through fan triangulation
      {
        area += calculateTriangleArea(pivotPoint, secondPoint, outPoints[i]);
        secondPoint = outPoints[i];
      }

      //outPolygonCentroid = vec3f::identity();
      //for(unsigned int i = 0; i < outPoints.size(); i++)
      //{
      //  outPolygonCentroid += outPoints[i];
      //}

      //outPolygonCentroid /= outPoints.size();

      //calculate the angle between the polygon and the z-axis to rotate the polygon so that its perpenidcular to the z plane
      vec3f polygonNormal = math::cross(outPoints[2] - outPoints[0], outPoints[1] - outPoints[0]).normalize();
      vec3f rotationAxis = math::cross(vec3f(0, 0, 1), polygonNormal);
      float zAxisAngle = acosf(math::clamp(vec3f::dot(vec3f(0, 0, 1), polygonNormal), -1.0f, 1.0f));
      Quaternion<float> rotQuat = math::createRotAxisQuaternion<float>(zAxisAngle, rotationAxis);
      Matrix<float, 4> rotMatrix = rotQuat.toMatrix();

      for(unsigned int i = 0; i < outPoints.size(); i++)//rotate the points parallel to the xy plane
      {
        outPoints[i] = rotMatrix * outPoints[i];
      }

      //calculate the centroid
      float cx = 0.0f;
      float cy = 0.0f;
      for(unsigned int i = 0; i < outPoints.size(); i++)//calculate the centroid of the convex polygon
      {
        unsigned int index = (i + 1) % outPoints.size();
        vec3f p0 = outPoints[i];
        vec3f p1 = outPoints[index];

        cx += (p0[0] + p1[0]) * (p0[0] * p1[1] - p1[0] * p0[1]);
        cy += (p0[1] + p1[1]) * (p0[0] * p1[1] - p1[0] * p0[1]);
      }

      float centroidArea = 1.0f / (6.0f * area);
      outPolygonCentroid[0] = centroidArea * cx;
      outPolygonCentroid[1] = centroidArea * cy;
      outPolygonCentroid[2] = outPoints[0][2];

      outPolygonCentroid = rotQuat.invert().toMatrix() * outPolygonCentroid;

      return area;
    }

    void PointCloudGenerator::generateCachesPerVoxel(unsigned int voxelIndex, std::vector<std::vector<float>> normalBin)
    {
      unsigned int  thetaQuantizer = 360 / normalBin[0].size();
      unsigned int  uQuantizer = normalBin[0].size();

      unsigned int emptyBins = 1;

      while(emptyBins)
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

        if(!emptyBins) break;

        vec2f angle0 = binToCylinder(vec2ui(maxBin[0], maxBin[1] == normalBin.size() ? normalBin.size() - 1 : maxBin[1]), thetaQuantizer, uQuantizer);
        vec2f angle1 = binToCylinder(vec2ui(maxBin[0] + 1, maxBin[1] + 1 > normalBin.size() ? normalBin.size() : maxBin[1] + 1), thetaQuantizer, uQuantizer);

        float newTheta = (angle0[0] + angle1[0]) * 0.5f;
        float newU = (angle0[1] + angle1[1]) * 0.5f;

        he::util::vec3f qNormal;
        qNormal[0] = sqrt(1.0f - newU * newU) * cosf(newTheta);//calculate normal-bin
        qNormal[1] = sqrt(1.0f - newU * newU) * sinf(newTheta);
        qNormal[2] = newU;

        std::list<PolygonData> cachePolygons;

        std::list<PolygonData>::iterator pit = m_polygons[voxelIndex].begin();
        while(pit != m_polygons[voxelIndex].end())
        {
          if(acosf(math::clamp(vec3f::dot(pit->normal, qNormal), -1.0f, 1.0f)) < m_maxAngle)//take the normal only if the angle to the normal-bin is not larger than 'maxAngle'
          {
            vec2ui bin = normalToBin(pit->normal, thetaQuantizer, uQuantizer);
            bin[0] = bin[0] % normalBin.size();
            bin[1] = bin[1] == normalBin.size() ? normalBin.size() - 1 : bin[1];
            normalBin[bin[0]][bin[1]] -= pit->area;
            cachePolygons.push_back(*pit);

            pit = m_polygons[voxelIndex].erase(pit);

            if(m_polygons[voxelIndex].empty()) break;
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

        PolygonData cache;
        cache.centroid = summedCentroid / summedArea;
        cache.normal = (summedNormal / summedArea).normalize();
        cache.area = summedArea;

        m_areaCaches[voxelIndex].push_back(cache);
      }
    }

    void PointCloudGenerator::shiftCentroid(unsigned int voxelIndex)
    {
      for(std::list<PolygonData>::iterator cit = m_areaCaches[voxelIndex].begin(); cit != m_areaCaches[voxelIndex].end(); cit++)
      {
        unsigned int bestTriangle = 0;
        vec3f nearestPoint;
        vec3f triangleVertices[3];
        float error = UINT_MAX;

        for(unsigned int i = 0; i < m_triangles[voxelIndex].size(); i += 3)
        {
          vec3f t0(m_triangles[voxelIndex][i]), t1(m_triangles[voxelIndex][i + 1]), t2(m_triangles[voxelIndex][i + 2]);
          vec3f normal = math::cross(t1 - t0, t2 - t0).normalize();
          vec3f tmpNearestPoint;
          float distance, tmpError;

          distance = calculatePointTriangleDistance(t0, t1, t2, cit->centroid, tmpNearestPoint);

          tmpError = distance * sqrt(math::clamp(1.0f - vec3f::dot(normal, cit->normal), 0.0f, 1.0f));

          if(error > tmpError)
          {
            error = tmpError;
            nearestPoint = tmpNearestPoint;
            triangleVertices[0] = t0;
            triangleVertices[1] = t1;
            triangleVertices[2] = t2;
          }
        }

        cit->centroid = nearestPoint;
        cit->triangleVertices[0] = triangleVertices[0];
        cit->triangleVertices[1] = triangleVertices[1];
        cit->triangleVertices[2] = triangleVertices[2];
      }
    }

    void PointCloudGenerator::reduceCaches()
    {
      for(std::map<unsigned int, std::list<PolygonData>>::iterator mip = m_linearizedAreaCaches.begin(); mip != m_linearizedAreaCaches.end(); mip++)
      {
        std::list<PolygonData>::iterator cit = mip->second.begin();
        while(cit != mip->second.end())
        {
          vec3ui voxelIndex3D(math::vector_cast<unsigned int>(math::abs<float>(cit->centroid - m_globalBBMin) / m_maxDistance));
          unsigned int voxelIndex = voxelIndex3D[0] * m_voxelNumber * m_voxelNumber + voxelIndex3D[1] * m_voxelNumber + voxelIndex3D[2];

          std::list<PolygonData> similarCaches;

          for(int nx = -1; nx < 2; nx++)
          {
            for(int ny = -1; ny < 2; ny++)
            {
              for(int nz = -1; nz < 2; nz++)
              {
                if(0 <= voxelIndex3D[0] + nx && voxelIndex3D[0] + nx < m_voxelNumber && 0 <= voxelIndex3D[1] + ny && voxelIndex3D[1] + ny < m_voxelNumber && 0 <= voxelIndex3D[2] + nz && voxelIndex3D[2] + nz < m_voxelNumber)
                {
                  unsigned int newVoxelIndex = voxelIndex + nx * m_voxelNumber * m_voxelNumber + ny * m_voxelNumber + nz;

                  std::list<PolygonData>::iterator vcit = m_areaCaches[newVoxelIndex].begin();
                  while(vcit != m_areaCaches[newVoxelIndex].end())
                  {
                    if(acosf(math::clamp(vec3f::dot(vcit->normal, cit->normal), -1.0f, 1.0f)) < m_maxAngle * m_errorRate && (vcit->centroid - cit->centroid).length() < m_maxDistance * m_errorRate)
                    {
                      if(vcit->area != cit->area || vcit->normal != cit->normal || vcit->centroid != cit->centroid)//is it diffrent to the actual cache?
                      {
                        for(std::list<PolygonData>::iterator dit = m_linearizedAreaCaches[newVoxelIndex].begin(); dit != m_linearizedAreaCaches[newVoxelIndex].end(); dit++)//if yes we need to erase the similar cache from the global iterating list so that we dont take the cache twice
                        {
                          if(vcit->area == dit->area && vcit->normal == dit->normal && vcit->centroid == dit->centroid)
                          {
                            m_linearizedAreaCaches[newVoxelIndex].erase(dit);//if we found the cache, remove it from the global iterating list
                            break;
                          }
                        }
                      }

                      similarCaches.push_back(*vcit);

                      vcit = m_areaCaches[newVoxelIndex].erase(vcit);

                      if(m_areaCaches[newVoxelIndex].empty()) break;
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

          cit = mip->second.erase(cit);

          PolygonData biggestCache;
          float maxArea = 0.0f;

          std::list<PolygonData>::iterator end = similarCaches.end();
          for(std::list<PolygonData>::iterator sit = similarCaches.begin(); sit != end; sit++)
          {
            if(sit->area > maxArea)
            {
              maxArea = sit->area;
              biggestCache = *sit;
            }
          }

          m_reducedCaches.push_back(biggestCache);
        }
      }
    }

    void PointCloudGenerator::sortCachesPerTriangle(const std::vector<vec3f>& positions)
    {
      for(unsigned int i = 0; i < positions.size(); i += 3)
      {
        vec3f t0 = positions[i];
        vec3f t1 = positions[i + 1];
        vec3f t2 = positions[i + 2];

        unsigned int startIndex = m_caches.size();
        for(unsigned int j = 0; j < m_reducedCaches.size(); j++)
        {
          if(math::abs(m_reducedCaches[j].triangleVertices[0] - t0) < vec3f(0.0001f, 0.0001f, 0.0001f) &&
            math::abs(m_reducedCaches[j].triangleVertices[1] - t1) < vec3f(0.0001f, 0.0001f, 0.0001f) &&
            math::abs(m_reducedCaches[j].triangleVertices[2] - t2) < vec3f(0.0001f, 0.0001f, 0.0001f))
          {
            Cache cache;
            cache.position = m_reducedCaches[j].centroid;
            cache.normal = m_reducedCaches[j].normal;
            m_caches.push_back(cache);
          }
        }

        m_triangleCacheIndices.push_back(vec2ui(startIndex, m_caches.size()));
      }
    }

    void PointCloudGenerator::generateCaches(std::vector<Cache>& outCaches, std::vector<vec2ui>& outTriangleCacheIndices, float errorRate, float maxDistance, float maxAngle, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices)
    {
      m_caches.clear();
      m_triangleCacheIndices.clear();

      m_errorRate = errorRate;
      m_maxDistance = maxDistance;
      m_maxAngle = maxAngle;

      generateAABB(positions, m_globalBBMin, m_globalBBMax);

      std::vector<vec3f> trianglePositions;

      if(!indices.empty())
      {
        createUnindexedTriangles(trianglePositions, positions, indices);
      }
      else
      {
        trianglePositions = positions;
      }

      vec3f boxSize(m_globalBBMax - m_globalBBMin);

      float sideMax = std::max(boxSize[0], std::max(boxSize[1], boxSize[2]));
      sideMax = maxDistance * pow(2.0f, ceil(log(sideMax / maxDistance) / log(2.0f)));

      m_globalBBMax[0] = m_globalBBMin[0] + sideMax;
      m_globalBBMax[1] = m_globalBBMin[1] + sideMax;
      m_globalBBMax[2] = m_globalBBMin[2] + sideMax;

      m_voxelNumber = ceil(sideMax / maxDistance);
      m_polygons.resize(m_voxelNumber * m_voxelNumber * m_voxelNumber);
      m_areaCaches.resize(m_voxelNumber * m_voxelNumber * m_voxelNumber);
      m_triangles.resize(m_voxelNumber * m_voxelNumber * m_voxelNumber);

      recursiveGenerateCaches(m_globalBBMin, m_globalBBMax, trianglePositions);

      reduceCaches();

      sortCachesPerTriangle(trianglePositions);

      m_triangles.clear();
      m_polygons.clear();
      m_areaCaches.clear();
      m_linearizedAreaCaches.clear();

      outCaches = m_caches;
      outTriangleCacheIndices = m_triangleCacheIndices;
    }
  }
}