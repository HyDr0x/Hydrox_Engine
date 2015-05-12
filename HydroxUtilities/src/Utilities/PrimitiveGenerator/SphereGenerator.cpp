#include "Utilities/PrimitiveGenerator/SphereGenerator.h"

namespace he
{
  namespace util
  {
    void SphereGenerator::generateSphere(float radius, std::vector<vec3f>& position, unsigned int levelOfDetail)
    {
      std::vector<vec3f> normal;
      generateSphere(radius, position, normal, levelOfDetail);
    }

    void SphereGenerator::generateSphere(float radius, std::vector<vec3f>& position, std::vector<vec3f>& normal, unsigned int levelOfDetail)
    {
      unsigned int triangleNumber = pow(4.0, static_cast<double>(levelOfDetail));//calculate number of position

      position.resize(triangleNumber);
      normal.resize(triangleNumber);

      createTetrahedron(radius, position, normal);

      //tesselation of the tetrahedron
      unsigned int triangleCount = 0, currentTriangles = 4;
      vec3f va, vb, vc;

      for(unsigned int i = 1; i < levelOfDetail; i++)
      {
        for(unsigned int j = 0; j < currentTriangles * 3; j += 3)
        {
          unsigned int index = (triangleCount + currentTriangles) * 3;

          va = position[j] + (position[j + 1] - position[j]) * 0.5f;
          vb = position[j] + (position[j + 2] - position[j]) * 0.5f;
          vc = position[j + 1] + (position[j + 2] - position[j + 1]) * 0.5f;

          va = va.normalize() * radius;
          vb = vb.normalize() * radius;
          vc = vc.normalize() * radius;

          position[index] = va;
          position[index + 1] = vb;
          position[index + 2] = position[j];
          normal[index] = position[index].normalize();
          normal[index + 1] = position[index + 1].normalize();
          normal[index + 2] = position[index + 2].normalize();

          position[index + 3] = va;
          position[index + 4] = vc;
          position[index + 5] = position[j + 1];
          normal[index + 3] = position[index + 3].normalize();
          normal[index + 4] = position[index + 4].normalize();
          normal[index + 5] = position[index + 5].normalize();

          position[index + 6] = vb;
          position[index + 7] = vc;
          position[index + 8] = position[j + 2];
          normal[index + 6] = position[index + 6].normalize();
          normal[index + 7] = position[index + 7].normalize();
          normal[index + 8] = position[index + 8].normalize();

          position[j] = va;
          position[j + 1] = vb;
          position[j + 2] = vc;
          normal[j] = position[j].normalize();
          normal[j + 1] = position[j + 1].normalize();
          normal[j + 2] = position[j + 2].normalize();

          triangleCount += 3;
        }
        currentTriangles += triangleCount;
        triangleCount = 0;
      }
    }

    void SphereGenerator::generateSphere(float radius, std::vector<vec3f>& position, std::vector<unsigned int>& indices, unsigned int levelOfDetail)
    {
      std::vector<vec3f> normal;
      generateSphere(radius, position, indices, normal, levelOfDetail);
    }

    void SphereGenerator::generateSphere(float radius, std::vector<vec3f>& position, std::vector<unsigned int>& indices, std::vector<vec3f>& normal, unsigned int levelOfDetail)
    {
      unsigned int triangleNumber = pow(4.0, static_cast<double>(levelOfDetail));//calculate number of position
      indices.resize(3 * triangleNumber);

      position.resize(4);
      normal.resize(4);

      createTetrahedron(radius, position, normal, indices);

      //tesselation of the tetrahedron
      unsigned int triangleCount = 0, currentTriangles = 4;

      for(unsigned int i = 1; i < levelOfDetail; i++)
      {
        for(unsigned int j = 0; j < currentTriangles * 3; j += 3)
        {
          unsigned int index = (triangleCount + currentTriangles) * 3;
          unsigned int vertexIndex = position.size();

          position.push_back(position[indices[j]] + (position[indices[j + 1]] - position[indices[j]]) * 0.5f);
          position.push_back(position[indices[j]] + (position[indices[j + 2]] - position[indices[j]]) * 0.5f);
          position.push_back(position[indices[j + 1]] + (position[indices[j + 2]] - position[indices[j + 1]]) * 0.5f);

          normal.push_back(position[vertexIndex].normalize());
          normal.push_back(position[vertexIndex + 1].normalize());
          normal.push_back(position[vertexIndex + 2].normalize());

          position[vertexIndex] = normal[vertexIndex] * radius;
          position[vertexIndex + 1] = normal[vertexIndex + 1] * radius;
          position[vertexIndex + 2] = normal[vertexIndex + 2] * radius;

          indices[index] = vertexIndex;
          indices[index + 1] = vertexIndex + 1;
          indices[index + 2] = indices[j];

          indices[index + 3] = vertexIndex;
          indices[index + 4] = indices[j + 1];
          indices[index + 5] = vertexIndex + 2;

          indices[index + 6] = vertexIndex + 1;
          indices[index + 7] = vertexIndex + 2;
          indices[index + 8] = indices[j + 2];

          indices[j] = vertexIndex;
          indices[j + 1] = vertexIndex + 2;
          indices[j + 2] = vertexIndex + 1;

          triangleCount += 3;
        }
        currentTriangles += triangleCount;
        triangleCount = 0;
      }
    }

    void SphereGenerator::createTetrahedron(float radius, std::vector<vec3f>& positions, std::vector<vec3f>& normals)
    {
      //build tetrahedron
      positions[0] = radius * vec3f(1, 1, -1).normalize();
      positions[1] = radius * vec3f(-1, -1, -1).normalize();
      positions[2] = radius * vec3f(-1, 1, 1).normalize();

      positions[3] = radius * vec3f(1, -1, 1).normalize();
      positions[4] = positions[1];
      positions[5] = positions[0];

      positions[6] = positions[0];
      positions[7] = positions[2];
      positions[8] = positions[3];

      positions[9] = positions[2];
      positions[10] = positions[1];
      positions[11] = positions[3];

      vec3f middle = 0.25f * (positions[0] + positions[1] + positions[2] + positions[3]);//calculate middle point of the tetrahedron

      //translate the tetrahedron into the middle
      for(unsigned int k = 0; k != 12; k++)
      {
        positions[k] -= middle;
      }

      //calculate normals
      for(int k = 0; k != 12; k++)
      {
        normals[k] = positions[k].normalize();
      }
    }

    void SphereGenerator::createTetrahedron(float radius, std::vector<vec3f>& positions, std::vector<vec3f>& normals, std::vector<unsigned int>& indices)
    {
      //build tetrahedron
      positions[0] = radius * vec3f(1, 1, -1).normalize();
      positions[1] = radius * vec3f(-1, -1, -1).normalize();
      positions[2] = radius * vec3f(-1, 1, 1).normalize();
      positions[3] = radius * vec3f(1, -1, 1).normalize();

      indices[0] = 0;
      indices[1] = 1;
      indices[2] = 2;

      indices[3] = 3;
      indices[4] = 1;
      indices[5] = 0;

      indices[6] = 0;
      indices[7] = 2;
      indices[8] = 3;

      indices[9] = 2;
      indices[10] = 1;
      indices[11] = 3;

      vec3f middle = 0.25f * (positions[0] + positions[1] + positions[2] + positions[3]);//calculate middle point of the tetrahedron

      //translate the tetrahedron into the middle
      for(unsigned int k = 0; k != 4; k++)
      {
        positions[k] -= middle;
      }

      //calculate normals
      for(int k = 0; k != 4; k++)
      {
        normals[k] = positions[k].normalize();
      }
    }
  }
}