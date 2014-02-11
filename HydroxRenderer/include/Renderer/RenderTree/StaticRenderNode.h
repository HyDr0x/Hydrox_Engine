#ifndef STATICRENDERNODE_H_
#define STATICRENDERNODE_H_

#include "Renderer/RenderTree/RenderNode.h"

namespace he
{
	namespace renderer
	{
    class Traverser;

    class StaticRenderNode : public RenderNode
    {
    public:

      StaticRenderNode();
      ~StaticRenderNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      TreeNode* createNewNode(InsertGeometryTraverser* traverser);

    private:

      void resizeMatrixBuffer();

      void fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex);
      void fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex);

      void uploadMatrixBuffer(sg::GeoNode *node, Shader *renderShader);
      void uploadMatrixBuffer(sg::AnimatedGeoNode *node, Shader *renderShader);
    };
	}
}

#endif
