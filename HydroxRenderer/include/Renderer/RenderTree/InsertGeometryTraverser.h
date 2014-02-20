#ifndef INSERTGEOMETRYTRAVERSER_H_
#define INSERTGEOMETRYTRAVERSER_H_

#include <vector>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/RenderTree/Traverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

namespace he
{
	namespace renderer
	{
    {
    public:

      InsertGeometryTraverser(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, util::ResourceHandle cullingShaderHandle);
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

      ComputeShaderManager *m_computeShaderManager;
      ModelManager *m_modelManager;
	    MaterialManager *m_materialManager;
	    RenderShaderManager *m_renderShaderManager;

      util::ResourceHandle m_cullingShaderHandle;

      TreeNode *m_parentNode;

      sg::GeoNode *m_node;

      unsigned int m_maxMaterials;
      unsigned int m_maxGeometry;
      unsigned int m_maxBones;

      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;
    };
	}
}

#endif