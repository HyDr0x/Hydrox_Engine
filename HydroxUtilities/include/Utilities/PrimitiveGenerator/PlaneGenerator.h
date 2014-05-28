#ifndef PLANEGENERATOR_H_
#define PLANEGENERATOR_H_

#include <vector>

#include "Utilities/DLLExport.h"

#include "Utilities/Math/Math.hpp"

namespace he
{
	namespace util
	{
    class GRAPHICAPI PlaneGenerator
    {
    public:

      static void generatePlane(std::vector<Vector<float, 3>>& position, unsigned int levelOfDetail);
      static void generatePlane(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail);

    private:

      static void generateIndexedPositions(std::vector<Vector<float, 3>>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail);
      static void generateNonIndexedPositions(std::vector<Vector<float, 3>>& position, unsigned int levelOfDetail);
      
      static void generateIndexedNormals(const std::vector<Vector<float, 3>>& position, std::vector<Vector<float, 3>>& normal);
      static void generateNonIndexedNormals(const std::vector<Vector<float, 3>>& position, std::vector<Vector<float, 3>>& normal);

      static void generateIndexedTextures(std::vector<Vector<float, 3>>& texture);
      static void generateNonIndexedTextures(std::vector<Vector<float, 3>>& texture);
    };
	}
}
#endif
