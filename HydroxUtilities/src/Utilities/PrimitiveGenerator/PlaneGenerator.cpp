#include "Utilities/PrimitiveGenerator/PlaneGenerator.h"

#include <vector>
#include <string>

namespace he
{
  namespace util
  {
    void PlaneGenerator::generatePlane(std::vector<Vector<float, 3>>& position, unsigned int levelOfDetail)
    {
      generateNonIndexedPositions(position, levelOfDetail);
    }

    void PlaneGenerator::generatePlane(std::vector<Vector<float, 3>>& position, std::vector<Vector<float, 3>>& normal, unsigned int levelOfDetail)
    {
      generateNonIndexedPositions(position, levelOfDetail);
      generateNonIndexedNormals(position, normal);
    }

    void PlaneGenerator::generatePlane(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail)
    {
      generateIndexedPositions(position, index, levelOfDetail);
    }

    void PlaneGenerator::generatePlane(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index, std::vector<Vector<float, 3>>& normal, unsigned int levelOfDetail)
    {
      generateIndexedPositions(position, index, levelOfDetail);
      generateIndexedNormals(position, index, normal);
    }
    
    void PlaneGenerator::generateIndexedPositions(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail)
    {
      unsigned int quadNumber = levelOfDetail + 1;
      float width = 1.0f / float(quadNumber);
      float height = 1.0f / float(quadNumber);

      unsigned int vertexNumber = quadNumber + 1;
      position.resize(vertexNumber * vertexNumber);
      
      for(unsigned int x = 0; x < vertexNumber; x++)
      {
        for(unsigned int z = 0; z < vertexNumber; z++)
        {
          unsigned int vertexIndex = x * vertexNumber + z;

          position[vertexIndex] = Vector<float, 3>(-0.5f + (x + 0) * width, 0.0f, -0.5f + (z + 0) * height);
        }
      }

      index.resize(quadNumber * quadNumber * 6);

      for(unsigned int x = 0; x < quadNumber; x++)
      {
        for(unsigned int z = 0; z < quadNumber; z++)
        {
          unsigned int indexIndex = (x * quadNumber + z) * 6;

          index[indexIndex + 0] = (x * vertexNumber) + z + vertexNumber;
          index[indexIndex + 1] = (x * vertexNumber) + z + 0;
          index[indexIndex + 2] = (x * vertexNumber) + z + 1;

          index[indexIndex + 3] = (x * vertexNumber) + z + vertexNumber;
          index[indexIndex + 4] = (x * vertexNumber) + z + 1;
          index[indexIndex + 5] = (x * vertexNumber) + z + vertexNumber + 1;
        }
      }
    }

    void PlaneGenerator::generateNonIndexedPositions(std::vector<Vector<float, 3>>& position, unsigned int levelOfDetail)
    {
      unsigned int quadNumber = levelOfDetail + 1;
      float width = 1.0f / float(quadNumber);
      float height = 1.0f / float(quadNumber);

      position.resize(quadNumber * quadNumber * 6);
      
      for(unsigned int x = 0; x < quadNumber; x++)
      {
        for(unsigned int z = 0; z < quadNumber; z++)
        {
          unsigned int index = (x * quadNumber + z) * 6;

          position[index + 0] = Vector<float, 3>(-0.5f + (x + 0) * width, 0.0f, -0.5f + (z + 0) * height);
          position[index + 1] = Vector<float, 3>(-0.5f + (x + 0) * width, 0.0f, -0.5f + (z + 1) * height);
          position[index + 2] = Vector<float, 3>(-0.5f + (x + 1) * width, 0.0f, -0.5f + (z + 1) * height);

          position[index + 3] = Vector<float, 3>(-0.5f + (x + 0) * width, 0.0f, -0.5f + (z + 0) * height);
          position[index + 4] = Vector<float, 3>(-0.5f + (x + 1) * width, 0.0f, -0.5f + (z + 1) * height);
          position[index + 5] = Vector<float, 3>(-0.5f + (x + 1) * width, 0.0f, -0.5f + (z + 0) * height);
        }
      }
    }
      
    void PlaneGenerator::generateIndexedNormals(const std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index, std::vector<Vector<float, 3>>& normal)
    {
      Vector<float, 3> a = position[index[1]] - position[index[0]];
      Vector<float, 3> b = position[index[2]] - position[index[0]];

      Vector<float, 3> n = math::cross(a, b).normalize();

      normal.resize(position.size());
      for(unsigned int i = 0; i < position.size(); i++)
      {
        normal[i] = n;
      }
    }

    void PlaneGenerator::generateNonIndexedNormals(const std::vector<Vector<float, 3>>& position, std::vector<Vector<float, 3>>& normal)
    {
      Vector<float, 3> a = position[1] - position[0];
      Vector<float, 3> b = position[2] - position[0];

      Vector<float, 3> n = math::cross(a, b).normalize();

      normal.resize(position.size());
      for(unsigned int i = 0; i < position.size(); i++)
      {
        normal[i] = n;
      }
    }

    void PlaneGenerator::generateIndexedTextures(std::vector<Vector<float, 3>>& texture)
    {
    }

    void PlaneGenerator::generateNonIndexedTextures(std::vector<Vector<float, 3>>& texture)
    {
    }
  }
}