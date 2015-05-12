#include "Utilities/Miscellaneous/SutherlandHodgman.h"

#include <vector>
#include <cstdint>
#include <algorithm>

#include "Utilities/Math/Math.hpp"
#include "Utilities/Math/Plane.h"
#include "Utilities/Miscellaneous/clipper.hpp"

namespace he
{
  namespace util
  {
    void sutherlandHodgman(Plane clippingPlane, const Polygon& polygon, float epsilon, Polygon& clippedPolygon)
    {
      std::vector<vec3f> outPoints;

      vec3f output;

      vec3f s = polygon.getPoints()[polygon.getPointNumber() - 1];
      for(unsigned int j = 0; j < polygon.getPointNumber(); j++)//all polygon edges
      {
        vec3f e = polygon.getPoints()[j];
        if(clippingPlane.isPointBehind(e, epsilon))
        {
          vec3f triangleEdge = s - e;
          if(clippingPlane.collisionLine(e, triangleEdge, output) && epsilon < output[0] && output[0] + epsilon < 1.0f)
          {
            outPoints.push_back(e + output[0] * triangleEdge);
          }

          outPoints.push_back(e);
        }
        else if(clippingPlane.isPointBehind(s, epsilon))
        {
          vec3f triangleEdge = s - e;
          if(clippingPlane.collisionLine(e, triangleEdge, output) && epsilon < output[0] && output[0] + epsilon < 1.0f)
          {
            outPoints.push_back(e + output[0] * triangleEdge);
          }
        }

        s = e;
      }

      clippedPolygon = Polygon(outPoints);
    }

    Polygon cutPolygonBox(std::vector<vec3f> inPoints, std::vector<vec3f> boxPoints, float epsilon)
    {
      Polygon inPolygon, outPolygon(inPoints);

      for(unsigned int i = 0; i < 6; i++)//all 6 box planes
      {
        inPolygon = outPolygon;
        outPolygon.clear();

        sutherlandHodgman(Plane(boxPoints[3 * i], boxPoints[3 * i + 1], boxPoints[3 * i + 2]), inPolygon, epsilon, outPolygon);

        if(outPolygon.getPointNumber() < 3)//polygon only touched the voxel
        {
          break;
        }
      }

      return outPolygon;
    }

    static bool polygonLineCollision(const Polygon& polygon, vec3f p, vec3f direction, vec3f& output, vec2ui& edgeIndices)
    {
      float epsilon = 0.00001f;

      output = vec3f(FLT_MAX, FLT_MAX, FLT_MAX);

      for(unsigned int j = 0; j < polygon.getPointNumber(); j++)//all polygon edges
      {
        vec3f tmpOutput;
        Plane clippingPlane(polygon.getPoints()[j], polygon.getPoints()[(j + 1) % polygon.getPointNumber()], polygon.getPoints()[j] + polygon.getNormal());
        if(clippingPlane.collisionLine(p, direction, tmpOutput) && epsilon <= tmpOutput[0] && tmpOutput[0] < 1.0f && 0.0f <= tmpOutput[1] && tmpOutput[1] < 1.0f &&
          vec3f::dot(clippingPlane.getNormal(), direction.normalize()) < 0.00001f)
        {
          edgeIndices = vec2ui(j, (j + 1) % polygon.getPointNumber());
          output = tmpOutput[0] < output[0] ? tmpOutput : output;
        }
      }

      return output != vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
    }

    static bool pointInsidePolygon(const Polygon& polygon, vec3f p)
    {
      for(unsigned int j = 0; j < polygon.getPointNumber(); j++)//all polygon edges
      {
        Plane clippingPlane(polygon.getPoints()[j], polygon.getPoints()[(j + 1) % polygon.getPointNumber()], polygon.getPoints()[j] + polygon.getNormal());
        float tmp = vec3f::dot(clippingPlane.getNormal(), (p - clippingPlane.getPosition()).normalize());
        if(vec3f::dot(clippingPlane.getNormal(), (p - clippingPlane.getPosition()).normalize()) > 0.00001f)
        {
          return false;
        }
      }

      for(unsigned int i = 0; i < polygon.getInsidePolygonNumber(); i++)//all inside polygons
      {
        if(!pointInsidePolygon(polygon.getInsidePolygons(i), p))
        {
          return false;
        }
      }

      return true;
    }

    static void getOutsidePolygonPoints(const Polygon& outsidePolygon, const Polygon& insidePolygon, std::vector<vec3ui>& startPointIndices, unsigned int polygonID)//it returns the points which lie interior of the outsidePolygon
    {
      for(unsigned int i = 0; i < insidePolygon.getPointNumber(); i++)//all polygon edges
      {
        if(!pointInsidePolygon(outsidePolygon, insidePolygon[i]))
        {
          startPointIndices.push_back(vec3ui(i, (i + 1) % insidePolygon.getPointNumber(), polygonID));
        }
      }
    }

    static bool tracePolygonRing(const Polygon& polygon0, const Polygon& polygon1, std::vector<Polygon>& outPolygons)
    {
      std::vector<vec3ui> startPointIndices0;//x = startPointIndex, y = nextPointIndex, z = polygon: 0 --> trace, 1 --> clipping
      std::vector<vec3ui> startPointIndices1;

      getOutsidePolygonPoints(polygon1, polygon0, startPointIndices0, 0);//are points of polygon0 outside of polygon1?
      getOutsidePolygonPoints(polygon0, polygon1, startPointIndices1, 1);//are points of polygon1 outside of polygon0?

      if(startPointIndices1.empty() && startPointIndices0.size() == polygon0.getPointNumber())
      {
        outPolygons.push_back(polygon0);
        return true;
      }
      else if(startPointIndices0.empty() && startPointIndices1.size() == polygon1.getPointNumber())
      {
        outPolygons.push_back(polygon1);
        return true;
      }

      bool collision = false;

      std::vector<Polygon> polygons(2);
      polygons[0] = polygon0;
      polygons[1] = polygon1;
      unsigned int tpIndex = 0, cpIndex = 1;

      vec3f linePoint, lineDirection;
      vec3ui startPointIndex, actualPointIndex;

      std::vector<std::vector<vec3f>> newPolyPoints;
      const unsigned int maxUnionPolyVertexNumber = polygon0.getPointNumber() + polygon1.getPointNumber() + floor(polygon0.getPointNumber() * 0.5f) * floor(polygon1.getPointNumber() * 0.5f) * 4.0f;

      std::vector<vec3ui>::iterator it = startPointIndices0.begin();
      while(it != startPointIndices0.end())//all polygon start points
      {
        startPointIndex = actualPointIndex = (*it);
        it = startPointIndices0.erase(it);

        linePoint = polygons[actualPointIndex[2]][actualPointIndex[0]];
        lineDirection = polygons[actualPointIndex[2]][actualPointIndex[1]] - linePoint;

        newPolyPoints.push_back(std::vector<vec3f>());

        do//all polygon edges
        {
          std::vector<vec3ui>::iterator pit;
          pit = std::find(startPointIndices0.begin(), startPointIndices0.end(), actualPointIndex);
          if(pit != startPointIndices0.end())
          {
            if(it == pit)
            {
              it = startPointIndices0.erase(pit);
            }
            else
            {
              startPointIndices0.erase(pit);
            }
          }

          if(newPolyPoints.back().size() >= maxUnionPolyVertexNumber)
          {
            outPolygons.push_back(polygon0);
            return false;
          }

          newPolyPoints.back().push_back(linePoint);

          vec2ui edgeIndices;
          vec3f output;
          if(polygonLineCollision(polygons[cpIndex], linePoint, lineDirection, output, edgeIndices))
          {
            collision = true;
            linePoint += output[0] * lineDirection;
            actualPointIndex = vec3ui(UINT32_MAX, UINT32_MAX, UINT32_MAX);//new created point has no indices in the old polygons

            vec3f traceLineNormal = math::cross(polygons[tpIndex].getNormal(), lineDirection);

            vec3f clippingEdgeStart = polygons[cpIndex][edgeIndices[0]], clippingEdgeEnd = polygons[cpIndex][edgeIndices[1]];

            lineDirection = clippingEdgeEnd - linePoint;
            actualPointIndex[1] = edgeIndices[1];

            /*if(vec3f::dot(traceLineNormal, (clippingEdgeEnd - clippingEdgeStart).normalize()) >= 0.0f)
            {
              lineDirection = clippingEdgeEnd - linePoint;
              actualPointIndex[1] = edgeIndices[1];
            }
            else
            {
              lineDirection = clippingEdgeStart - linePoint;
              actualPointIndex[1] = edgeIndices[0];
            }*/

            tpIndex = 1 - tpIndex;
            cpIndex = 1 - cpIndex;
          }
          else
          {
            linePoint += lineDirection;
            actualPointIndex = vec3ui(actualPointIndex[1], (actualPointIndex[1] + 1) % polygons[tpIndex].getPointNumber(), tpIndex);
            lineDirection = polygons[tpIndex][actualPointIndex[1]] - linePoint;
          }
        } while(startPointIndex != actualPointIndex);
      }

      for(unsigned int i = 0; i < newPolyPoints.size(); i++)//delete nearly doubled points
      {
        unsigned int id = 0;
        std::vector<vec3f>::iterator it = newPolyPoints[i].begin();
        while(it != newPolyPoints[i].end())
        {
          bool erased = false;
          if((*it - newPolyPoints[i][(id + 1) % newPolyPoints[i].size()]).length() < 0.00001f)
          {
            erased = true;
            it = newPolyPoints[i].erase(it);
            break;
          }

          if(!erased)
          {
            it++;
            id++;
          }
        }
      }

      for(unsigned int i = 0; i < newPolyPoints.size(); i++)//all new polygons
      {
        outPolygons.push_back(Polygon(newPolyPoints[i]));
      }

      return collision;
    }

    

    bool polygonUnion2D(Polygon tracePolygon, Polygon clippingPolygon, Polygon& outPolygon)
    {
      bool collision = false;

      std::vector<Polygon> outPolygons;
      collision = tracePolygonRing(tracePolygon, clippingPolygon, outPolygons);

      unsigned int outsidePolygonIndex = 0;
      Polygon clipPolygon(outPolygons[outsidePolygonIndex]);

      for(unsigned int i = 0; i < outPolygons.size(); i++)
      {
        if(i != outsidePolygonIndex)
        {
          for(unsigned int j = 0; j < outPolygons[i].getPointNumber(); j++)//all polygon edges
          {
            if(!pointInsidePolygon(clipPolygon, outPolygons[i][j]))//if a point is not inside the polygon, it can't be the outside polygon
            {
              clipPolygon = Polygon(outPolygons[i]);
              outsidePolygonIndex = i;
              i = ~0;
              break;
            }
          }
        }
      }

      std::vector<Polygon> insidePolygons;
      for(unsigned int i = 0; i < outPolygons.size(); i++)
      {
        if(i != outsidePolygonIndex)
        {
          if(outPolygons[i].getArea() > 0.0f)
          {
            outPolygons[i].invertWindingOrder();//change the winding order of the intern polygons if necessary
          }
          insidePolygons.push_back(outPolygons[i]);
        }
      }

      for(unsigned int i = 0; i < tracePolygon.getInsidePolygonNumber(); i++)
      {
        if(tracePolygonRing(tracePolygon.getInsidePolygons(i), clippingPolygon, insidePolygons))//inside polygons can only be interior polygons
        {
          if(insidePolygons.back() == clippingPolygon)//if the clipping polygon covers the inside polygon complete, the hole is closed
          {
            insidePolygons.erase(insidePolygons.end());//remove the hole
          }
          collision = true;
        }
      }

      outPolygon = Polygon(outPolygons[outsidePolygonIndex].getPoints(), insidePolygons);
      return collision;
    }
  }
}
