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
    std::list<Cache> PointCloudGenerator::generateCaches(float errorRate, float maxDistance, float maxAngle, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices)
    {
      std::vector<std::vector<vec3f>> triangles;//saves all triangles for each voxel, 3 x xyz per triangle in a list
      std::list<Cache> caches;//saves all caches per voxel
      std::vector<std::list<PolygonData>> polygons;//saves all polygons which lying in the voxel and then the caches with their area

      vec3f bbMin, bbMax;

      generateAABB(positions, bbMin, bbMax);

      float boxSize = maxDistance * 0.5f;
      float boxHalfSize = boxSize * 0.5f;
      vec3f aabbCenter = (bbMax - bbMin) * 0.5f;

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

      vec3i voxelNumber((bbMax[0] - bbMin[0]) / boxSize + 1, (bbMax[1] - bbMin[1]) / boxSize + 1, (bbMax[2] - bbMin[2]) / boxSize + 1);
      unsigned int triangleNumber = indices.empty() ? triangleNumber = positions.size() : triangleNumber = indices.size();

      triangles.resize(voxelNumber[0] * voxelNumber[1] * voxelNumber[2]);
      polygons.resize(voxelNumber[0] * voxelNumber[1] * voxelNumber[2]);

      for(unsigned int x = 0; x < voxelNumber[0]; x++)
      {
        for(unsigned int y = 0; y < voxelNumber[1]; y++)
        {
          for(unsigned int z = 0; z < voxelNumber[2]; z++)
          {
            vec3f boxCenter(bbMin[0] + x * boxSize + boxHalfSize, bbMin[1] + y * boxSize + boxHalfSize, bbMin[2] + z * boxSize + boxHalfSize);
            unsigned int voxelIndex = x * voxelNumber[1] * voxelNumber[2] + y * voxelNumber[2] + z;

            
            std::vector<std::vector<float>> normalBin(18);//area of the triangles which have that normal

            for(unsigned int i = 0; i < normalBin.size(); i++)
            {
              normalBin[i].resize(normalBin.size(), 0.0f);
            }

            unsigned int  thetaQuantizer = 360 / normalBin[0].size();
            unsigned int  uQuantizer = normalBin[0].size();

            for(unsigned int i = 0; i < triangleNumber; i += 3)
            {
              vec3f t0, t1, t2;
              if(indices.empty())
              {
                t0 = positions[i];
                t1 = positions[i + 1];
                t2 = positions[i + 2];
              }
              else
              {
                t0 = positions[indices[i]];
                t1 = positions[indices[i + 1]];
                t2 = positions[indices[i + 2]];
              }

              vec3f v0(t0 - boxCenter), v1(t1 - boxCenter), v2(t2 - boxCenter);

              vec3f e0 = v1 - v0;
              vec3f e1 = v2 - v0;
              vec3f e2 = v2 - v1;

              vec3f normal = math::cross(e0, e1).normalize();

              if(fastTriangleCenteredBoxTest(v0, v1, v2, normal, vec3f(boxHalfSize, boxHalfSize, boxHalfSize)))
              {
                triangles[voxelIndex].push_back(t0);
                triangles[voxelIndex].push_back(t1);
                triangles[voxelIndex].push_back(t2);

                std::vector<vec3f> trianglePoints(3);
                trianglePoints[0] = v0;
                trianglePoints[1] = v1;
                trianglePoints[2] = v2;

                vec3f polygonCentroid;

                float area = calculatePolygonAreaCentroid(trianglePoints, boxPoints, polygonCentroid);//calculate area of the part of the triangle which lies in the voxel an return the centroid of that polygon

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
                polygons[voxelIndex].push_back(data);
              }
            }

            if(!polygons.empty())
            {
              generateCachesPerVoxel(polygons[voxelIndex], normalBin, maxAngle);
              shiftCentroid(polygons[voxelIndex], triangles[voxelIndex]);
            }
          }
        }
      }

      reduceCaches(errorRate, maxAngle, maxDistance, voxelNumber, polygons, caches);

      return caches;
    }

    void removeSimilarCaches(float errorRate, float maxAngle, float maxDistance, vec3i voxelNumber, unsigned int voxelIndex, vec3ui voxelCoords, PointCloudGenerator::PolygonData cache, std::vector<std::list<PointCloudGenerator::PolygonData>>& caches, std::list<Cache>& outCaches)
    {
      std::list<PointCloudGenerator::PolygonData> similarCaches;

      for(int nx = -1; nx < 2; nx++)
      {
        for(int ny = -1; ny < 2; ny++)
        {
          for(int nz = -1; nz < 2; nz++)
          {
            if(0 <= voxelCoords[0] + nx && voxelCoords[0] + nx < voxelNumber[0] && 0 <= voxelCoords[1] + ny && voxelCoords[1] + ny < voxelNumber[1] && 0 <= voxelCoords[2] + nz && voxelCoords[2] + nz < voxelNumber[2])
            {
              unsigned int newVoxelIndex = voxelIndex + nx * voxelNumber[1] * voxelNumber[2] + ny * voxelNumber[2] + nz;

              std::list<PointCloudGenerator::PolygonData>::iterator cit = caches[newVoxelIndex].begin();
              while(cit != caches[newVoxelIndex].end())
              {
                if(acosf(math::clamp(vec3f::dot(cit->normal, cache.normal), -1.0f, 1.0f)) < maxAngle * errorRate && (cit->centroid - cache.centroid).length() < maxDistance * errorRate)
                {
                  similarCaches.push_back(*cit);

                  cit = caches[newVoxelIndex].erase(cit);

                  if(caches[newVoxelIndex].empty()) break;
                }
                else
                {
                  cit++;
                }
              }
            }
          }
        }
      }

      PointCloudGenerator::PolygonData biggestCache;
      float maxArea = 0.0f;

      std::list<PointCloudGenerator::PolygonData>::iterator end = similarCaches.end();
      for(std::list<PointCloudGenerator::PolygonData>::iterator cit = similarCaches.begin(); cit != end; cit++)
      {
        if(cit->area > maxArea)
        {
          maxArea = cit->area;
          biggestCache = *cit;
        }
      }

      Cache bestCache;
      bestCache.position = biggestCache.centroid;
      bestCache.normal = biggestCache.normal;
      outCaches.push_back(bestCache);
    }

    void PointCloudGenerator::reduceCaches(float errorRate, float maxAngle, float maxDistance, vec3i voxelNumber, std::vector<std::list<PolygonData>>& caches, std::list<Cache>& outCaches)
    {
      for(unsigned int x = 0; x < voxelNumber[0]; x++)
      {
        for(unsigned int y = 0; y < voxelNumber[1]; y++)
        {
          for(unsigned int z = 0; z < voxelNumber[2]; z++)
          {
            unsigned int voxelIndex = x * voxelNumber[1] * voxelNumber[2] + y * voxelNumber[2] + z;
            while(!caches[voxelIndex].empty())
            {
              removeSimilarCaches(errorRate, maxAngle, maxDistance, voxelNumber, voxelIndex, vec3ui(x, y, z), *caches[voxelIndex].begin(), caches, outCaches);
            }
          }
        }
      }
    }

    void PointCloudGenerator::shiftCentroid(std::list<PolygonData>& caches, std::vector<vec3f>& triangles)
    {
      for(std::list<PolygonData>::iterator cit = caches.begin(); cit != caches.end(); cit++)
      {
        unsigned int bestTriangle = 0;
        vec3f nearestPoint;
        float error = UINT_MAX;

        for(unsigned int i = 0; i < triangles.size(); i += 3)
        {
          vec3f t0(triangles[i]), t1(triangles[i + 1]), t2(triangles[i + 2]);
          vec3f normal = math::cross(t1 - t0, t2 - t0).normalize();
          vec3f tmpNearestPoint;
          float distance, tmpError;

          distance = calculatePointTriangleDistance(t0, t1, t2, cit->centroid, tmpNearestPoint);

          tmpError = distance * sqrt(math::clamp(1.0f - vec3f::dot(normal, cit->normal), 0.0f, 1.0f));

          if(error > tmpError)
          {
            error = tmpError;
            nearestPoint = tmpNearestPoint;
          }
        }

        cit->centroid = nearestPoint;
      }
    }

    void PointCloudGenerator::generateCachesPerVoxel(std::list<PolygonData>& polygons, std::vector<std::vector<float>> normalBin, float maxAngle)
    {
      std::list<PolygonData> outCaches;

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
            if(normalBin[i][j] <= 0.00001f)
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

        std::list<PolygonData>::iterator pit = polygons.begin();
        while(pit != polygons.end())
        {
          if(acosf(math::clamp(vec3f::dot(pit->normal, qNormal), -1.0f, 1.0f)) < maxAngle)//take the normal only if the angle to the normal-bin is not larger than 'maxAngle'
          {
            vec2ui bin = normalToBin(pit->normal, thetaQuantizer, uQuantizer);
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

        PolygonData cache;
        cache.centroid = summedCentroid / summedArea;
        cache.normal = (summedNormal / summedArea).normalize();
        cache.area = summedArea;

        outCaches.push_back(cache);
      }

      polygons = outCaches;
    }

    void PointCloudGenerator::generateAABB(const std::vector<vec3f>& positions, vec3f& bbMin, vec3f& bbMax)
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

    float PointCloudGenerator::calculatePolygonAreaCentroid(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints, vec3f& outPolygonCentroid)
    {
      float epsilon = 0.00001f;
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
          if(vec3f::dot(planeNormal, e - boxPoints[3 * i]) <= epsilon)
          {
            vec3f triangleEdge = s - e;
            if(linePlaneCollision(e, triangleEdge, boxPoints[3 * i], boxPoints[3 * i + 1], boxPoints[3 * i + 2], output) && epsilon < output[0] && output[0] + epsilon < 1.0f)
            {
              outPoints.push_back(e + output[0] * triangleEdge);
            }

            outPoints.push_back(e);
          }
          else if(vec3f::dot(planeNormal, s - boxPoints[3 * i]) <= epsilon)
          {
            vec3f triangleEdge = s - e;
            if(linePlaneCollision(e, triangleEdge, boxPoints[3 * i], boxPoints[3 * i + 1], boxPoints[3 * i + 2], output) && epsilon < output[0] && output[0] + epsilon < 1.0f)
            {
              outPoints.push_back(e + output[0] * triangleEdge);
            }
          }

          s = e;
        }
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
      outPolygonCentroid = centroidArea * vec3f(cx, cy, 0.0f);
      outPolygonCentroid[2] = outPoints[0][2];

      outPolygonCentroid = rotQuat.invert().toMatrix() * outPolygonCentroid;

      return area;
    }
  }
}