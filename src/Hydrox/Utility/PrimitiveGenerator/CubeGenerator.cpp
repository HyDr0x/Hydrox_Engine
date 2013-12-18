#include "Hydrox/Utility/PrimitiveGenerator/CubeGenerator.h"

#include <vector>
#include <string>

#include "Hydrox/Utility/Math/Math.hpp"

namespace he
{
  CubeGenerator::CubeGenerator()
  {
  }

  CubeGenerator::~CubeGenerator()
  {
  }

  Mesh CubeGenerator::generateCube(ResourceHandle materialHandle)
  {
    std::vector<Vector<float, 3>> position(8);

    position[0] = Vector<float, 3>(-0.5f, -0.5f, -0.5f);
    position[1] = Vector<float, 3>( 0.5f, -0.5f, -0.5f);
    position[2] = Vector<float, 3>(-0.5f, -0.5f,  0.5f);
    position[3] = Vector<float, 3>( 0.5f, -0.5f,  0.5f);

    position[4] = Vector<float, 3>(-0.5f,  0.5f, -0.5f);
    position[5] = Vector<float, 3>( 0.5f,  0.5f, -0.5f);
    position[6] = Vector<float, 3>(-0.5f,  0.5f,  0.5f);
    position[7] = Vector<float, 3>( 0.5f,  0.5f,  0.5f);

    std::vector<Mesh::indexType> index(36);//CCW is OGL's default front facing

    index[0] = 0;//front
    index[1] = 1;
    index[2] = 4;

    index[3] = 4;
    index[4] = 1;
    index[5] = 5;

    index[6] = 5;//right
    index[7] = 1;
    index[8] = 3;

    index[9] = 5;
    index[10] = 3;
    index[11] = 7;

    index[12] = 4;//left
    index[13] = 6;
    index[14] = 2;

    index[15] = 4;
    index[16] = 2;
    index[17] = 0;

    index[18] = 7;//back
    index[19] = 3;
    index[20] = 6;

    index[21] = 6;
    index[22] = 3;
    index[23] = 2;

    index[24] = 7;//top
    index[25] = 6;
    index[26] = 4;

    index[27] = 7;
    index[28] = 4;
    index[29] = 5;

    index[30] = 3;//down
    index[31] = 2;
    index[32] = 0;

    index[33] = 3;
    index[34] = 0;
    index[35] = 1;

    return Mesh(Mesh::MODEL_POSITION, position, GL_TRIANGLES, index);
  }
}