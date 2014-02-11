#ifndef RENDERNODE_H_
#define RENDERNODE_H_

#include <list>
#include <map>

#include <Utilities/Math/Math.hpp>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/Buffer/UBO.h"
#include "Renderer/Buffer/SSBO.h"

#include "Renderer/RenderTree/TreeNode.h"

namespace he
{
	namespace renderer
	{
    class Traverser;

    class RenderNode : public TreeNode
    {
    public:

      RenderNode();
      virtual ~RenderNode() = 0;

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual TreeNode* createNewNode(InsertGeometryTraverser* traverser);

      void initialize(RenderShaderManager *renderShaderManager, MaterialManager *materialManager, ModelManager *modelManager);

      void insertGeometry(sg::GeoNode *node);
      bool removeGeometry(sg::GeoNode *node);

      void rasterizeGeometry();

    protected:

      void resizeBuffer();

      virtual void resizeMatrixBuffer() = 0;

      virtual void fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex) = 0;
      virtual void fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex) = 0;

      virtual void uploadMatrixBuffer(sg::GeoNode *node, Shader *renderShader) = 0;
      virtual void uploadMatrixBuffer(sg::AnimatedGeoNode *node, Shader *renderShader) = 0;//DEPRECATED --> should be in the resizeBuffer() method

      RenderShaderManager *m_renderShaderManager;
      MaterialManager *m_materialManager;
      ModelManager *m_modelManager;

      //gpu buffer for drawing
      UBO m_materialBuffer;
      UBO m_materialIndexBuffer;
      SSBO m_matrixBuffer;

      std::map<unsigned int, unsigned int> m_materialHandles;

      bool m_geometryChanged;
      unsigned int m_maxMaterials;

      //geoNodes cache, if they change they can be reuploaded fast
      std::list<sg::GeoNode*> m_geoNodes;
    };
	}
}

#endif
