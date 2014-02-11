#ifndef GEOMETRYRASTERIZER_H_
#define GEOMETRYRASTERIZER_H_

#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include "Renderer/Buffer/UBO.h"

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/RenderTree/GroupNode.h"

namespace he
{
	namespace renderer
	{
    class GeometryRasterizer
    {
    public:

      GeometryRasterizer(util::SingletonManager *singletonManager, unsigned int nodeCacheBlockSize = 8);
      ~GeometryRasterizer();

      void insertGeometry(sg::GeoNode *node);

      void removeGeometry(sg::GeoNode *node);

      void rasterizeGeometry(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition);

    private:

      GLuint m_simpleMeshVAO;

      UBO m_cameraParameterUBO;

      GroupNode *m_renderRootNode;

      ModelManager *m_modelManager;
	    MaterialManager *m_materialManager;
	    RenderShaderManager *m_renderShaderManager;
      TextureManager *m_textureManager;
    };
	}
}

#endif
