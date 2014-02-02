#ifndef CUBEGENERATOR_H_
#define CUBEGENERATOR_H_

#include <vector>

#include "Utilities/DLLExport.h"

#include "Utilities/Math/Math.hpp"

namespace he
{
	namespace util
	{
    class GRAPHICAPI CubeGenerator
    {
    public:

      CubeGenerator();
      ~CubeGenerator();

      static void generateCube(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index);

    private:

    };
	}
}
#endif
