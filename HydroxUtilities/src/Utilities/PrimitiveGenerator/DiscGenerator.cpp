#include "Utilities/PrimitiveGenerator/DiscGenerator.h"

#include <vector>
#include <string>

namespace he
{
  namespace util
  {
    void DiscGenerator::generateDisc(float radius, vec3f direction, std::vector<vec3f>& position, unsigned int vertexNumber)
    {
      generateNonIndexedPositions(radius, direction, position, vertexNumber);
    }

    void DiscGenerator::generateDisc(float radius, vec3f direction, std::vector<vec3f>& position, std::vector<vec3f>& normal, unsigned int vertexNumber)
    {
      generateNonIndexedPositions(radius, direction, position, vertexNumber);
      generateNormals(direction, normal, vertexNumber + 1);
    }

    void DiscGenerator::generateDisc(float radius, vec3f direction, std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int vertexNumber)
    {
      generateIndexedPositions(radius, direction, position, index, vertexNumber);
    }

    void DiscGenerator::generateDisc(float radius, vec3f direction, std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec3f>& normal, unsigned int vertexNumber)
    {
      generateIndexedPositions(radius, direction, position, index, vertexNumber);
      generateNormals(direction, normal, vertexNumber + 1);
    }

    void DiscGenerator::generateIndexedPositions(float radius, vec3f direction, std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int vertexNumber)
    {
      vec3f xDir = math::cross(vec3f(1, 2, 3), direction).normalize();
      vec3f yDir = math::cross(xDir, direction).normalize();

      float angleStride = (2.0f * math::PI) / float(vertexNumber);
      float angle = 0.0f;
      unsigned int i0 = 0, i1 = 0;

      position.push_back(vec3f::identity());

      for(unsigned int i = 0; i < vertexNumber; i++)
      {
        i0 = i1;
        i1 = position.size();

        position.push_back(radius * (xDir * cosf(angle) + yDir * sinf(angle)));

        angle -= angleStride;

        if(position.size() >= 3)
        {
          index.push_back(0);
          index.push_back(i0);
          index.push_back(i1);
        }
      }

      index.push_back(0);
      index.push_back(i1);
      index.push_back(1);
    }

    void DiscGenerator::generateNonIndexedPositions(float radius, vec3f direction, std::vector<vec3f>& position, unsigned int vertexNumber)
    {
      vec3f xDir = math::cross(vec3f(1, 2, 3), direction).normalize();
      vec3f yDir = math::cross(xDir, direction).normalize();

      float angleStride = (2.0f * math::PI) / float(vertexNumber);
      float angle = angleStride;

      for(unsigned int i = 0; i < vertexNumber; i++)
      {
        position.push_back(vec3f::identity());
        position.push_back(radius * (xDir * cosf(angle - angleStride) + yDir * sinf(angle - angleStride)));
        position.push_back(radius * (xDir * cosf(angle) + yDir * sinf(angle)));

        angle += angleStride;
      }
    }

    void DiscGenerator::generateNormals(vec3f direction, std::vector<vec3f>& normal, unsigned int size)
    {
      for(unsigned int i = 0; i < size; i++)
      {
        normal.push_back(direction);
      }
    }

    void DiscGenerator::generateIndexedTextures(vec3f direction, std::vector<vec3f>& texture)
    {
    }

    void DiscGenerator::generateNonIndexedTextures(vec3f direction, std::vector<vec3f>& texture)
    {
    }
  }
}