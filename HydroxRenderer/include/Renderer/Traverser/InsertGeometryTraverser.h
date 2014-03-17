#ifndef INSERTGEOMETRYTRAVERSER_H_
#define INSERTGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"

namespace he
{
  namespace xBar  {    class StaticGeometryContainer;
    class SkinnedGeometryContainer;  }

	namespace renderer
	{    class IRenderNode;    class InsertGeometryTraverser : public Traverser
    {
    public:

      InsertGeometryTraverser(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, std::list<IRenderNode*>& renderNodesStatic, xBar::StaticGeometryContainer& geometryContainer, bool skinned);
      virtual ~InsertGeometryTraverser();

      virtual bool preTraverse(GroupNode* treeNode);
      virtual void postTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(IRenderNode* treeNode);
      virtual void postTraverse(IRenderNode* treeNode);

      void createNewChildNode(TreeNode* parent);
      void createNewChildNode(GroupNode* parent);
      void createNewChildNode(VertexDeclarationNode* parent);
      void createNewChildNode(ShaderNode* parent);
      void createNewChildNode(TextureNode* parent);

      void createNewSibling(TreeNode* sibling);
      void createNewSibling(VertexDeclarationNode* sibling);
      void createNewSibling(ShaderNode* sibling);
      void createNewSibling(TextureNode* sibling);
      void createNewSibling(IRenderNode* sibling);

    private:

      util::SingletonManager *m_singletonManager;

      std::list<IRenderNode*>& m_renderNodes;

      bool m_inserted;

      xBar::StaticGeometryContainer& m_geometryContainer;

      unsigned int m_maxMaterials;
      unsigned int m_maxGeometry;
      unsigned int m_maxBones;

      RenderNodeType m_nodeType;
      GLenum m_primitiveType;
      GLuint m_vertexStride;
      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;
    };
	}
}

#endif
