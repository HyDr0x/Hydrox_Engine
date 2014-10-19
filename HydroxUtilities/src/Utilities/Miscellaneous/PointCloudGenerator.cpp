#include "Utilities/Miscellaneous/PointCloudGenerator.h"

#include <random>
#include <cmath>

#include "Utilities/Math/CollisionTests.h"
#include "Utilities/Miscellaneous/SutherlandHodgman.hpp"

namespace he
{
  namespace util
  {
    std::vector<std::list<PointCloudGenerator::Cache>> PointCloudGenerator::generateCaches(float maxDistance, float maxAngle, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices)
    {
      generateAABB(positions);

      float boxSize = maxDistance * 0.5f;
      float boxHalfSize = boxSize * 0.5f;
      vec3f aabbCenter = (m_bbMax - m_bbMin) * 0.5f;

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
      vec3i voxelNumber((m_bbMax[0] - m_bbMin[0]) / boxSize + 1, (m_bbMax[1] - m_bbMin[1]) / boxSize + 1, (m_bbMax[2] - m_bbMin[2]) / boxSize + 1);

      unsigned int triangleNumber = 0;

      if(indices.empty())
      {
        triangleNumber = positions.size() / 3;

        for(unsigned int x = 0; x < voxelNumber[0]; x++)
        {
          for(unsigned int y = 0; y < voxelNumber[1]; y++)
          {
            for(unsigned int z = 0; z < voxelNumber[2]; z++)
            {
              vec3f boxCenter(x * boxSize + boxHalfSize - aabbCenter[0], y * boxSize + boxHalfSize - aabbCenter[1], z * boxSize + boxHalfSize - aabbCenter[2]);
              unsigned int voxelIndex = x * voxelNumber[1] * voxelNumber[2] + y * voxelNumber[2] + z;

              std::list<PolygonData> polygons;//saves all polygons which lying in the voxel
              std::vector<std::vector<float>> normalBin(18);//area of the triangles which have that normal

              for(unsigned int i = 0; i < normalBin.size(); i++)
              {
                normalBin[i].resize(normalBin.size(), 0.0f);
              }

              unsigned int  thetaQuantizer = 360 / normalBin[0].size();
              unsigned int  uQuantizer = normalBin[0].size();

              for(unsigned int i = 0; i < triangleNumber; i++)
              {
                vec3f boxCenter(x + boxHalfSize, y + boxHalfSize, z + boxHalfSize);
                vec3f v0(positions[i] - boxCenter), v1(positions[i + 1] - boxCenter), v2(positions[i + 2] - boxCenter);

                vec3f e0 = v1 - v0;
                vec3f e1 = v2 - v0;
                vec3f e2 = v2 - v1;

                vec3f normal = math::cross(e0, e1).normalize();

                bool contains = triangleCenteredBoxTest(v0, v1, v2, normal, vec3f(boxHalfSize, boxHalfSize, boxHalfSize));

                if(contains)
                {
                  m_triangles[voxelIndex].push_back(positions[i]);
                  m_triangles[voxelIndex].push_back(positions[i + 1]);
                  m_triangles[voxelIndex].push_back(positions[i + 2]);

                  normal = normal.normalize();
                  vec2ui bin = normalToBin(normal, thetaQuantizer, uQuantizer);
                  
                  std::vector<vec3f> trianglePoints(3);
                  trianglePoints[0] = v0;
                  trianglePoints[1] = v1;
                  trianglePoints[2] = v2;

                  vec3f polygonCentroid;

                  float area = calculatePolygonAreaCentroid(trianglePoints, boxPoints, polygonCentroid);//calculate area of the part of the triangle which lies in the voxel an return the centroid of that polygon

                  PolygonData data;
                  data.area = area;
                  data.centroid = polygonCentroid;
                  data.normal = normal;

                  normalBin[bin[0]][bin[1]] += area;
                  polygons.push_back(data);
                }
              }

              generateCachesPerVoxel(polygons, normalBin, maxAngle, m_caches[voxelIndex]);
            }
          }
        }
      }
      else
      {
        triangleNumber = indices.size() / 3;
      }

      return m_caches;
    }

    void PointCloudGenerator::generateCachesPerVoxel(std::list<PolygonData> polygons, std::vector<std::vector<float>> normalBin, float maxAngle, std::list<Cache>& outCaches)
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

        vec2f angle0 = binToCylinder(maxBin[0], thetaQuantizer, maxBin[1], uQuantizer);
        vec2f angle1 = binToCylinder((maxBin[0] + 1) % normalBin.size(), thetaQuantizer, (maxBin[1] + 1) % normalBin.size(), uQuantizer);

        float newTheta = (angle0[0] + angle1[0]) * 0.5f;
        float newU = (angle0[1] + angle1[1]) * 0.5f;

        he::util::vec3f qNormal;
        qNormal[0] = sqrt(1.0f - newU * newU) * cosf(newTheta);//calculate normal-bin
        qNormal[1] = sqrt(1.0f - newU * newU) * sinf(newTheta);
        qNormal[2] = newU;

        std::list<PolygonData> cachePolygons;
        for(std::list<PolygonData>::iterator it = polygons.begin(); it != polygons.end(); it++)
        {
          if(vec3f::dot(it->normal, qNormal) < maxAngle)//take the normal only if the angle to the normal-bin is not larger than 'maxAngle'
          {
            vec2ui bin = normalToBin(it->normal, thetaQuantizer, uQuantizer);
            normalBin[bin[0]][bin[1]] -= it->area;
            cachePolygons.push_back(*it);
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

        Cache cache;
        cache.position = summedCentroid / summedArea;
        cache.normal = summedNormal / summedArea;

        outCaches.push_back(cache);
      }
    }

    void PointCloudGenerator::generateAABB(const std::vector<vec3f>& positions)
    {
      m_bbMin = util::vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
      m_bbMax = util::vec3f(FLT_MIN, FLT_MIN, FLT_MIN);

      for(unsigned int i = 0; i < positions.size(); i++)
      {
        for(unsigned int j = 0; j < 3; j++)
        {
          if(m_bbMin[j] > positions[i][j])
          {
            m_bbMin[j] = positions[i][j];
          }

          if(m_bbMax[j] < positions[i][j])
          {
            m_bbMax[j] = positions[i][j];
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

      //calculate the angle between the polygon and the z-axis to rotate the polygon so that its perpenidcular to the z plane
      vec3f polygonNormal = math::cross(outPoints[2] - outPoints[0], outPoints[1] - outPoints[0]).normalize();
      vec3f rotationAxis = math::cross(vec3f(0, 0, 1), polygonNormal);
      float zAxisAngle = acosf(vec3f::dot(vec3f(0, 0, 1), polygonNormal));
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

      outPolygonCentroid = rotQuat.invert().toMatrix() * outPolygonCentroid;

      return area;
    }
  }
}