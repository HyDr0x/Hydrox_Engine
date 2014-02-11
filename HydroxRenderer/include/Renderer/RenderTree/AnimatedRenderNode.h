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

      AnimatedRenderNode();
      ~AnimatedRenderNode();

      bool preTraverse(Traverser* traverser);
      void postTraverse(Traverser* traverser);

      TreeNode* createNewNode(InsertGeometryTraverser* traverser);

    private:

      void resizeMatrixBuffer();

      void fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex);
      void fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex);

      void uploadMatrixBuffer(sg::GeoNode *node, Shader *renderShader);
      void uploadMatrixBuffer(sg::AnimatedGeoNode *node, Shader *renderShader);

      unsigned int m_maxBones;

      GLuint m_boneMatricesBuffer;
    };
	}
}

#endif
