#ifndef ANIMATEDRENDERNODE_H_
#define ANIMATEDRENDERNODE_H_

#include "Renderer/RenderTree/RenderNode.h"

namespace he
{
	namespace renderer
	{
    class Traverser;

    class AnimatedRenderNode : public RenderNode
    {
    public:

      AnimatedRenderNode(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones);
      ~AnimatedRenderNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      TreeNode* createNewNode(InsertGeometryTraverser* traverser);

    private:

      void resizeMatrixBuffer();

      void fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex);
      void fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex);

      void uploadMatrices();

      unsigned int m_maxBones;
    };
	}
}

#endif
