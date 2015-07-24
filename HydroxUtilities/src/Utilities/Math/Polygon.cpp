#include "Utilities/Math/Polygon.h"

#include <cassert>

#include <math.h>

#include "Utilities/Math/Plane.h"
#include "Utilities/Math/Triangle.h"

namespace he
{
  namespace util
  {
    Polygon::Polygon() : m_points(std::vector<vec3f>()), m_insidePolygons(std::vector<Polygon>())
    {
      calculateAttributes();
    }

    Polygon::Polygon(std::vector<vec3f> points, std::vector<Polygon> insidePolygons) : m_points(points), m_insidePolygons(insidePolygons)
    {
      calculateAttributes();
    }

    void Polygon::invertWindingOrder()
    {
      std::vector<vec3f> points(m_points.size());

      for(unsigned int i = 0; i < m_points.size(); i++)
      {
        points[i] = m_points[m_points.size() - i - 1];
      }

      m_points = points;

      calculateAttributes();
    }

    float Polygon::calculatePointPolygonDistance(vec3f point, vec3f& nearestPoint)
    {
      vec3f pointPlanePos;

      Plane plane(m_points[0], m_points[1], m_points[2]);
      plane.collisionLine(point, m_normal, pointPlanePos);

      if(0.0f <= pointPlanePos[1] && pointPlanePos[1] <= 1.0f && 0.0f <= pointPlanePos[2] && pointPlanePos[2] <= 1.0f && pointPlanePos[1] + pointPlanePos[2] <= 1.0f)
      {
        vec3f direction = pointPlanePos[0] * m_normal;
        nearestPoint = point + direction;
        return direction.length();//point lies, projected on the triangle plane, in the triangle
      }
      else
      {
        vec3f edge, pivotPoint;;

        unsigned int index;

        float distance = FLT_MAX;
        float pointProj;

        for(unsigned int i = 0; i < m_points.size(); i++)
        {
          index = (i + 1) % m_points.size();

          edge = m_points[index] - m_points[i];
          float edgeLength = edge.length();
          edge = edge.normalize();

          /* lineProj0 = vec3f::dot(m_points[i] - m_points[i], edge);
          lineProj1 = vec3f::dot(m_points[index] - m_points[i], edge);*/
          pointProj = vec3f::dot(point - m_points[i], edge);

          if(0 < pointProj && pointProj < edgeLength)//is the projection on the line between the polygon points which spans the line?
          {
            pivotPoint = m_points[i] + pointProj * edge;
          }
          else if(pointProj < 0)
          {
            pivotPoint = m_points[i];
          }
          else
          {
            pivotPoint = m_points[index];
          }

          float tmpDistance = (pivotPoint - point).length();
          if(distance > tmpDistance)
          {
            nearestPoint = pivotPoint;
            distance = tmpDistance;
          }
        }

        return distance;
      }
    }

    bool Polygon::operator==(const Polygon& other) const
    {
      return m_points == other.m_points && m_insidePolygons == other.m_insidePolygons;
    }

    const vec3f& Polygon::operator[] (unsigned int i) const
    {
      assert(i < m_points.size());

      return m_points[i];
    }

    unsigned int Polygon::getPointNumber() const
    {
      return m_points.size();
    }

    unsigned int Polygon::getInsidePolygonNumber() const
    {
      return m_insidePolygons.size();
    }

    const Polygon& Polygon::getInsidePolygons(unsigned int insidePolygonIndex) const
    {
      return m_insidePolygons[insidePolygonIndex];
    }

    void Polygon::setInsidePoints(unsigned int insidePolygonIndex, std::vector<vec3f> points)
    {
      m_insidePolygons[insidePolygonIndex].setPoints(points);

      calculateAttributes();
    }

    void Polygon::setInsidePoint(unsigned int insidePolygonIndex, unsigned int i, vec3f point)
    {
      m_insidePolygons[insidePolygonIndex].setPoint(i, point);

      calculateAttributes();
    }

    const std::vector<vec3f>& Polygon::getPoints() const
    {
      return m_points;
    }

    void Polygon::setPoints(std::vector<vec3f> points)
    {
      m_points = points;

      calculateAttributes();
    }

    void Polygon::setPoint(unsigned int i, vec3f v)
    {
      assert(i < m_points.size());

      m_points[i] = v;

      calculateAttributes();
    }

    std::vector<vec3f> Polygon::getRotatedPolygonPoints(util::vec3f planeNormal) const
    {
      //calculate the angle between the polygon and the z-axis to rotate the polygon so that its perpenidcular to the z plane
      vec3f rotationAxis = math::cross(planeNormal, m_normal).normalize();
      float zAxisAngle = acosf(math::clamp(vec3f::dot(planeNormal, m_normal), -1.0f, 1.0f));
      Quaternion<float> rotQuat = math::createRotAxisQuaternion<float>(zAxisAngle, rotationAxis);

      std::vector<vec3f> rotatedPoints(m_points.size());
      for(unsigned int i = 0; i < m_points.size(); i++)//rotate the points parallel to the xy plane
      {
        rotatedPoints[i] = rotQuat.apply(m_points[i]);
      }

      return rotatedPoints;
    }

    void Polygon::clear()
    {
      m_points.clear();
      m_insidePolygons.clear();

      calculateAttributes();
    }

    vec3f Polygon::getCentroid() const
    {
      return m_centroid;
    }

    vec3f Polygon::getNormal() const
    {
      return m_normal;
    }

    float Polygon::getArea() const
    {
      return m_area;
    }

    void Polygon::calculateAttributes()
    {
      m_normal = vec3f::identity();
      m_centroid = vec3f::identity();
      m_area = 0.0f;

      if(m_points.size() < 3)
      {
        return;
      }

      vec3f pivotPoint(m_points[0]), secondPoint(m_points[1]);
      std::vector<float> triangleAreas;
      std::vector<vec3f> triangleCentroids;
      vec3f normal = vec3f::identity();

      for(unsigned int i = 2; i < m_points.size(); i++)//triangulation through fan triangulation, only for convex polygons!
      {
        Triangle triangle(pivotPoint, secondPoint, m_points[i]);
        m_area += triangle.getArea();
        triangleAreas.push_back(m_area);

        triangleCentroids.push_back((pivotPoint + secondPoint + m_points[i]) / 3.0f);
        normal += triangle.getNormal();
        secondPoint = m_points[i];
      }

      m_normal = normal.normalize();

      for(unsigned int i = 0; i < triangleCentroids.size(); i++)//centroid calculation only for convex polygons!
      {
        m_centroid[0] += triangleCentroids[i][0] * triangleAreas[i];
        m_centroid[1] += triangleCentroids[i][1] * triangleAreas[i];
        m_centroid[2] += triangleCentroids[i][2] * triangleAreas[i];
      }

      m_centroid[0] /= m_area;
      m_centroid[1] /= m_area;
      m_centroid[2] /= m_area;
    }

    float Polygon::calculateConcaveArea() const
    {
      float area = 0.0f;

      std::vector<vec3f> rotatedPoints = getRotatedPolygonPoints(vec3f(0, 0, 1));

      for(unsigned int i = 0; i < rotatedPoints.size(); i++)//calculate the area with the gauss area formula
      {
        unsigned int index = (i + 1) % rotatedPoints.size();
        area += (rotatedPoints[i][1] + rotatedPoints[index][1]) * (rotatedPoints[i][0] - rotatedPoints[index][0]);
      }
      area *= 0.5f;

      for(unsigned int i = 0; i < m_insidePolygons.size(); i++)//add the negative areas of the inside polygons
      {
        area += m_insidePolygons[i].getArea();
      }

      return area;

      //vec3f e0 = m_points[1] - m_points[0];
      //vec3f e1 = m_points[2] - m_points[0];

      //float tmpArea = float(math::cross(e0, e1).length()) * 0.5f;
    }
  }
}