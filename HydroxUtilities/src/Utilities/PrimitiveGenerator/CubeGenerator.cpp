#include "Utilities/PrimitiveGenerator/CubeGenerator.h"

#include <vector>
#include <string>

namespace he
{
  namespace util
  {
    void CubeGenerator::generateCube(std::vector<Vector<float, 3>>& position)
    {
      generateNonIndexedPositions(position);
    }

    void CubeGenerator::generateCube(std::vector<Vector<float, 3>>& position, std::vector<Vector<float, 3>>& normal)
    {
      generateNonIndexedPositions(position);
      generateNonIndexedNormals(normal, position);
    }

    void CubeGenerator::generateCube(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index)
    {
      generateIndexedPositions(position, index);
    }

    void CubeGenerator::generateCube(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index, std::vector<Vector<float, 3>>& normal)
    {
      generateIndexedPositions(position, index);
      generateIndexedNormals(normal, position);
    }

    void CubeGenerator::generateIndexedPositions(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index)
    {
      position.resize(8);

      position[0] = Vector<float, 3>(-0.5f, -0.5f, -0.5f);
      position[1] = Vector<float, 3>( 0.5f, -0.5f, -0.5f);
      position[2] = Vector<float, 3>(-0.5f, -0.5f,  0.5f);
      position[3] = Vector<float, 3>( 0.5f, -0.5f,  0.5f);

      position[4] = Vector<float, 3>(-0.5f,  0.5f, -0.5f);
      position[5] = Vector<float, 3>( 0.5f,  0.5f, -0.5f);
      position[6] = Vector<float, 3>(-0.5f,  0.5f,  0.5f);
      position[7] = Vector<float, 3>( 0.5f,  0.5f,  0.5f);

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

    void CubeGenerator::generateNonIndexedPositions(std::vector<Vector<float, 3>>& position)
    {
      position.resize(36);
      //front
      position[0] = Vector<float, 3>(-0.5f, -0.5f, -0.5f);
      position[1] = Vector<float, 3>( 0.5f,  0.5f, -0.5f);
      position[2] = Vector<float, 3>( 0.5f, -0.5f, -0.5f);

      position[3] = Vector<float, 3>(-0.5f, -0.5f, -0.5f);
      position[4] = Vector<float, 3>(-0.5f,  0.5f, -0.5f);
      position[5] = Vector<float, 3>( 0.5f,  0.5f, -0.5f);

      //back
      position[6] = Vector<float, 3>(-0.5f, -0.5f, 0.5f);
      position[7] = Vector<float, 3>( 0.5f, 0.5f,  0.5f);
      position[8] = Vector<float, 3>(-0.5f, 0.5f,  0.5f);

      position[9] = Vector<float, 3>(-0.5f, -0.5f, 0.5f); 
      position[10] = Vector<float, 3>(0.5f, -0.5f, 0.5f); 
      position[11] = Vector<float, 3>(0.5f, 0.5f, 0.5f);

      //right
      position[12] = Vector<float, 3>(-0.5f, -0.5f, 0.5f);
      position[13] = Vector<float, 3>(-0.5f, 0.5f, -0.5f);
      position[14] = Vector<float, 3>(-0.5f, -0.5f, -0.5f); 

      position[15] = Vector<float, 3>(-0.5f, -0.5f, 0.5f);
      position[16] = Vector<float, 3>(-0.5f, 0.5f, 0.5f);
      position[17] = Vector<float, 3>(-0.5f, 0.5f, -0.5f);

      //left
      position[18] = Vector<float, 3>(0.5f, 0.5f, -0.5f);
      position[19] = Vector<float, 3>(0.5f, -0.5f, 0.5f);
      position[20] = Vector<float, 3>(0.5f, -0.5f, -0.5f);

      position[21] = Vector<float, 3>( 0.5f,  0.5f, -0.5f);
      position[22] = Vector<float, 3>( 0.5f,  0.5f,  0.5f);
      position[23] = Vector<float, 3>( 0.5f, -0.5f,  0.5f);

      //bottom
      position[24] = Vector<float, 3>(-0.5f, -0.5f, -0.5f);
      position[25] = Vector<float, 3>(0.5f, -0.5f, 0.5f);
      position[26] = Vector<float, 3>(-0.5f, -0.5f, 0.5f);

      position[27] = Vector<float, 3>(-0.5f, -0.5f, -0.5f);
      position[28] = Vector<float, 3>( 0.5f, -0.5f, -0.5f);
      position[29] = Vector<float, 3>( 0.5f, -0.5f,  0.5f);

      //top
      position[30] = Vector<float, 3>(-0.5f,  0.5f, -0.5f);
      position[31] = Vector<float, 3>( 0.5f,  0.5f,  0.5f);
      position[32] = Vector<float, 3>( 0.5f,  0.5f, -0.5f);

      position[33] = Vector<float, 3>(-0.5f,  0.5f, -0.5f);
      position[34] = Vector<float, 3>(-0.5f,  0.5f,  0.5f);
      position[35] = Vector<float, 3>( 0.5f,  0.5f,  0.5f);
    }

    void CubeGenerator::generateIndexedNormals(std::vector<Vector<float, 3>>& normal, const std::vector<Vector<float, 3>>& positions)
    {
      normal.resize(8);
      for(unsigned int i = 0; i < positions.size(); i++)
      {
        normal[i] = positions[i].normalize();
      }
    }

    void CubeGenerator::generateNonIndexedNormals(std::vector<Vector<float, 3>>& normal, const std::vector<Vector<float, 3>>& positions)
    {
      normal.resize(36);

      for(unsigned int i = 0; i < positions.size(); i += 3)
      {
        Vector<float, 3> a = positions[i + 1] - positions[i];
        Vector<float, 3> b = positions[i + 2] - positions[i];
        normal[i] = normal[i + 1] = normal[i + 2] = math::cross(a, b).normalize();
      }
    }

    void CubeGenerator::generateIndexedTextures(std::vector<Vector<float, 3>>& texture)
    {
    }

    void CubeGenerator::generateNonIndexedTextures(std::vector<Vector<float, 3>>& texture)
    {
    }
  }
}