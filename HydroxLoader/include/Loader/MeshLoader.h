#ifndef MESHLOADER_H_
#define MESHLOADER_H_

#include <map>

#include "Loader/ResourceLoader.h"

namespace he
{
  namespace loader
  {
    class GRAPHICAPI MeshLoader : public ResourceLoader
    {
    public:

      MeshLoader(float errorRate, float maxDistance, float maxAngle, util::SingletonManager *singletonManager);

      util::ResourceHandle loadResource(std::string filename);

      util::ResourceHandle getDefaultResource() const;

    private:

      struct MeshMetaData
      {
        util::vec3f bbMax;
        util::vec3f bbMin;
        GLuint primitiveType;
        GLuint primitiveCount;
        GLuint indexCount;
        GLuint vertexCount;
        GLuint vertexDeclaration;
        GLuint vertexStride;
        GLuint vboSize;
        GLuint cacheSize;
      };

      float m_errorRate;
      float m_maxDistance;
      float m_maxAngle;

      util::SharedPointer<db::ModelManager> m_modelManager;
    };
  }
}

#endif
