#ifndef SUTHERLANDHODGMAN_H_
#define SUTHERLANDHODGMAN_H_

#include <vector>

#include "Utilities/Math/Math.hpp"

#include "Utilities/Math/Plane.h"
#include "Utilities/Math/Polygon.h"

namespace he
{
  namespace util
  {
    void sutherlandHodgman(Plane clippingPlane, const Polygon& polygon, Polygon& clippedPolygon);//construct a polygon that lies behind the plane
    bool polygonUnion2D(Polygon tracePolygon, Polygon clippingPolygon, Polygon& outPolygon);//constructs the union from two planar polygons that lie in the same plane, returns true if they have been connected and false if not (means no colision)
    Polygon cutPolygonBox(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints);
  }
}

#endif