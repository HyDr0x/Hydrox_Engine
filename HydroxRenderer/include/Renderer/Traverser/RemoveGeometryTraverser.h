#ifndef REMOVEGEOMETRYTRAVERSER_H_
#define REMOVEGEOMETRYTRAVERSER_H_

#include <vector>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

namespace he
{
	namespace renderer
	{    class RemoveGeometryTraverser : public Traverser
    {
    public:

      RemoveGeometryTraverser(util::SingletonManager *singletonManager, std::list<RenderNode*>& renderNodes);
      virtual ~RemoveGeometryTraverser();

      void setNode(sg::GeoNode *node);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      void doTraverseDown(TreeNode* treeNode);

      void deleteNode(TreeNode *treeNode);

      ModelManager *m_modelManager;
	    MaterialManager *m_materialManager;

      std::list<RenderNode*>& m_renderNodes;

      sg::GeoNode *m_node;

      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;
    };
	}
}

#endif
