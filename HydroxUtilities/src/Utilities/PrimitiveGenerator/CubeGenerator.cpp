#include "Utilities/PrimitiveGenerator/CubeGenerator.h"

#include <vector>
#include <string>

namespace he
{
  namespace util
  {
    void CubeGenerator::generateCube(std::vector<vec3f>& position)
    {
      generateNonIndexedPositions(position);
    }

    void CubeGenerator::generateCube(std::vector<vec3f>& position, std::vector<vec3f>& normal)
    {
      generateNonIndexedPositions(position);
      generateNonIndexedNormals(normal, position);
    }

    void CubeGenerator::generateCube(std::vector<vec3f>& position, std::vector<unsigned int>& index)
    {
      generateIndexedPositions(position, index);
    }

    void CubeGenerator::generateCube(std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec3f>& normal)
    {
      generateIndexedPositions(position, index);
      generateIndexedNormals(normal, position);
    }

    void CubeGenerator::generateIndexedPositions(std::vector<vec3f>& position, std::vector<unsigned int>& index)
    {
      position.resize(8);

      position[0] = vec3f(-0.5f, -0.5f, -0.5f);
      position[1] = vec3f( 0.5f, -0.5f, -0.5f);
      position[2] = vec3f(-0.5f, -0.5f,  0.5f);
      position[3] = vec3f( 0.5f, -0.5f,  0.5f);

      position[4] = vec3f(-0.5f,  0.5f, -0.5f);
      position[5] = vec3f( 0.5f,  0.5f, -0.5f);
      position[6] = vec3f(-0.5f,  0.5f,  0.5f);
      position[7] = vec3f( 0.5f,  0.5f,  0.5f);

      index.resize(36);//CCW is OGL's default front facing

      index[0] = 0;//front
      index[1] = 4;
      index[2] = 1;

      index[3] = 4;
      index[4] = 5;
      index[5] = 1;

      index[6] = 5;//right
      index[7] = 3;
      index[8] = 1;

      index[9] = 5;
      index[10] = 7;
      index[11] = 3;

      index[12] = 4;//left
      index[13] = 2;
      index[14] = 6;

      index[15] = 4;
      index[16] = 0;
      index[17] = 2;

      index[18] = 7;//back
      index[19] = 6;
      index[20] = 3;

      index[21] = 6;
      index[22] = 2;
      index[23] = 3;

      index[24] = 7;//top
      index[25] = 4;
      index[26] = 6;

      index[27] = 7;
      index[28] = 5;
      index[29] = 4;

      index[30] = 3;//down
      index[31] = 2;
      index[32] = 0;

      index[33] = 3;
      index[34] = 0;
      index[35] = 1;
    }

    void CubeGenerator::generateNonIndexedPositions(std::vector<vec3f>& position)
    {
      position.resize(36);
      //front
      position[0] = vec3f(-0.5f, -0.5f, -0.5f);
      position[1] = vec3f( 0.5f,  0.5f, -0.5f);
      position[2] = vec3f( 0.5f, -0.5f, -0.5f);

      position[3] = vec3f(-0.5f, -0.5f, -0.5f);
      position[4] = vec3f(-0.5f,  0.5f, -0.5f);
      position[5] = vec3f( 0.5f,  0.5f, -0.5f);

      //back
      position[6] = vec3f(-0.5f, -0.5f, 0.5f);
      position[7] = vec3f( 0.5f, 0.5f,  0.5f);
      position[8] = vec3f(-0.5f, 0.5f,  0.5f);

      position[9] = vec3f(-0.5f, -0.5f, 0.5f); 
      position[10] = vec3f(0.5f, -0.5f, 0.5f); 
      position[11] = vec3f(0.5f, 0.5f, 0.5f);

      //right
      position[12] = vec3f(-0.5f, -0.5f, 0.5f);
      position[13] = vec3f(-0.5f, 0.5f, -0.5f);
      position[14] = vec3f(-0.5f, -0.5f, -0.5f); 

      position[15] = vec3f(-0.5f, -0.5f, 0.5f);
      position[16] = vec3f(-0.5f, 0.5f, 0.5f);
      position[17] = vec3f(-0.5f, 0.5f, -0.5f);

      //left
      position[18] = vec3f(0.5f, 0.5f, -0.5f);
      position[19] = vec3f(0.5f, -0.5f, 0.5f);
      position[20] = vec3f(0.5f, -0.5f, -0.5f);

      position[21] = vec3f( 0.5f,  0.5f, -0.5f);
      position[22] = vec3f( 0.5f,  0.5f,  0.5f);
      position[23] = vec3f( 0.5f, -0.5f,  0.5f);

      //bottom
      position[24] = vec3f(-0.5f, -0.5f, -0.5f);
      position[25] = vec3f(0.5f, -0.5f, 0.5f);
      position[26] = vec3f(-0.5f, -0.5f, 0.5f);

      position[27] = vec3f(-0.5f, -0.5f, -0.5f);
      position[28] = vec3f( 0.5f, -0.5f, -0.5f);
      position[29] = vec3f( 0.5f, -0.5f,  0.5f);

      //top
      position[30] = vec3f(-0.5f,  0.5f, -0.5f);
      position[31] = vec3f( 0.5f,  0.5f,  0.5f);
      position[32] = vec3f( 0.5f,  0.5f, -0.5f);

      position[33] = vec3f(-0.5f,  0.5f, -0.5f);
      position[34] = vec3f(-0.5f,  0.5f,  0.5f);
      position[35] = vec3f( 0.5f,  0.5f,  0.5f);
    }

    void CubeGenerator::generateIndexedNormals(std::vector<vec3f>& normal, const std::vector<vec3f>& positions)
    {
      normal.resize(8);
      for(unsigned int i = 0; i < positions.size(); i++)
      {
        normal[i] = positions[i].normalize();
      }
    }

    void CubeGenerator::generateNonIndexedNormals(std::vector<vec3f>& normal, const std::vector<vec3f>& positions)
    {
      normal.resize(36);

      for(unsigned int i = 0; i < positions.size(); i += 3)
      {
        vec3f a = positions[i + 1] - positions[i];
        vec3f b = positions[i + 2] - positions[i];
        normal[i] = normal[i + 1] = normal[i + 2] = math::cross(a, b).normalize();
      }
    }

    void CubeGenerator::generateIndexedTextures(std::vector<vec3f>& texture)
    {
    }

    void CubeGenerator::generateNonIndexedTextures(std::vector<vec3f>& texture)
    {
    }
  }
}