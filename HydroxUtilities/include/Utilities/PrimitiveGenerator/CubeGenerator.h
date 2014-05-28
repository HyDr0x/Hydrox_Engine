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

      static void generateCube(std::vector<Vector<float, 3>>& position);
      static void generateCube(std::vector<Vector<float, 3>>& position, std::vector<Vector<float, 3>>& normal);
      static void generateCube(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index);
      static void generateCube(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index, std::vector<Vector<float, 3>>& normal);

    private:

      static void generateIndexedPositions(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index);
      static void generateNonIndexedPositions(std::vector<Vector<float, 3>>& position);

      static void generateIndexedNormals(std::vector<Vector<float, 3>>& normal, const std::vector<Vector<float, 3>>& positions);
      static void generateNonIndexedNormals(std::vector<Vector<float, 3>>& normal, const std::vector<Vector<float, 3>>& positions);

      static void generateIndexedTextures(std::vector<Vector<float, 3>>& texture);
      static void generateNonIndexedTextures(std::vector<Vector<float, 3>>& texture);
    };
	}
}
#endif
