#include "Utilities/PrimitiveGenerator/CubeGenerator.h"

#include <vector>
#include <string>

namespace he
{
	namespace util
	{
    void CubeGenerator::generateCube(std::vector<Vector<float, 3>>& position)
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

    void CubeGenerator::generateCube(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index)
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
    }
	}
}