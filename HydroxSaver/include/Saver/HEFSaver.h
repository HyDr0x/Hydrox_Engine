#ifndef HEFSAVER_H_
#define HEFSAVER_H_

#include "Saver/DLLExport.h"

#include "Saver/NodeWrapper.h"
#include "Saver/NodeWrapperMapper.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace sg
  {
    class Scene;
  }

	namespace saver
	{
    struct NodeNumbers
    {
      unsigned int geoNodeSize;
      unsigned int animatedGeoNodeSize;
      unsigned int transformNodeSize;
      unsigned int animatedTransformNodeSize;
      unsigned int billboardNodeSize;
      unsigned int lodNodeSize;
      unsigned int lightNodeSize;
      unsigned int particleNodeSize;
    };

    struct MeshMetaData
    {
      util::Vector<float, 3> bbMax;
      util::Vector<float, 3> bbMin;
      GLuint primitiveType;
      GLuint primitiveCount;
      GLuint indexCount;
      GLuint vertexCount;
      GLuint vertexDeclaration;
      GLuint vertexStride;
      GLuint vboSize;
    };

    class GRAPHICAPI HEFSaver
    {
    public:

      ~HEFSaver();

      void save(std::string path, std::string filename, sg::Scene* scene, util::SingletonManager *singletonManager);

    private:

      void writeToFile(std::string path, std::string filename, util::SingletonManager *singletonManager);

      NodeWrapperMapper m_wrapperMapper;
    };
	}
}

#endif
