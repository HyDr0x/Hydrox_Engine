#ifndef INSERTGEOMETRYTRAVERSER_H_
#define INSERTGEOMETRYTRAVERSER_H_

#include <vector>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/RenderTree/Traverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

namespace he
{
	namespace renderer
	{    class InsertGeometryTraverser : public Traverser
    {
    public:

      InsertGeometryTraverser(ModelManager *modelManager, MaterialManager *materialManager, RenderShaderManager *renderShaderManager);
      virtual ~InsertGeometryTraverser();

      void setNode(sg::GeoNode *node);

      void doTraverse(TreeNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(StaticRenderNode* treeNode);
      virtual void postTraverse(StaticRenderNode* treeNode);

      virtual bool preTraverse(AnimatedRenderNode* treeNode);
      virtual void postTraverse(AnimatedRenderNode* treeNode);

      TreeNode* createNewNode(TreeNode* parent);
      TreeNode* createNewNode(GroupNode* parent);
      TreeNode* createNewNode(VertexDeclarationNode* parent);
      TreeNode* createNewNode(ShaderNode* parent);
      TreeNode* createNewNode(TextureNode* parent);

    protected:

      void doTraverseDown(TreeNode* treeNode);

    private:

      ModelManager *m_modelManager;
	    MaterialManager *m_materialManager;
	    RenderShaderManager *m_renderShaderManager;

      TreeNode *m_parentNode;

      sg::GeoNode *m_node;

      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;
    };
	}
}

#endif
