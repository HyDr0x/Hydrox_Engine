#ifndef POLYGON_H_
#define POLYGON_H_

#include <vector>

#include "Utilities/Math/math.hpp"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI Polygon
    {
    public:

      Polygon();
      Polygon(std::vector<vec3f> points, std::vector<Polygon> insidePolygons = std::vector<Polygon>());

      void invertWindingOrder();

      float calculatePointPolygonDistance(vec3f point, vec3f& nearestPoint);

      bool operator==(const Polygon& other) const;

      const vec3f& operator[] (unsigned int i) const;

      unsigned int getPointNumber() const;
      unsigned int getInsidePolygonNumber() const;

      const Polygon& getInsidePolygons(unsigned int insidePolygonIndex) const;
      void setInsidePoints(unsigned int insidePolygonIndex, std::vector<vec3f> points);
      void setInsidePoint(unsigned int insidePolygonIndex, unsigned int i, vec3f point);

      const std::vector<vec3f>& getPoints() const;
      void setPoints(std::vector<vec3f> points);
      void setPoint(unsigned int i, vec3f point);

      std::vector<vec3f> getRotatedPolygonPoints(util::vec3f planeNormal) const;

      void clear();

      vec3f getCentroid() const;

      vec3f getNormal() const;

      float getArea() const;

      float calculateConcaveArea() const;

    private:

      void calculateAttributes();

      std::vector<vec3f> m_points;//the points which define the polygon
      std::vector<Polygon> m_insidePolygons;//the polygons which define holes the polygon
      vec3f m_centroid;//centroid of the polygon
      vec3f m_normal;//normalized normal
      float m_area;//area of the polygon
    };
  }
}

#endif