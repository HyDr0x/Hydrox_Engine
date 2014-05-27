#ifndef INSERTGEOMETRYTRAVERSER_H_
#define INSERTGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace xBar  {    class StaticGeometryContainer;
    class SkinnedGeometryContainer;  }

	namespace renderer
	{    class IRenderNode;    class InsertGeometryTraverser : public Traverser
    {
    public:

      InsertGeometryTraverser(const RenderOptions& options, util::SingletonManager *singletonManager);
      virtual ~InsertGeometryTraverser() = 0;

      virtual bool preTraverse(GroupNode* treeNode);
      virtual void postTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(IRenderNode* treeNode) = 0;
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

    protected:

      void initialize(Mesh* mesh, util::ResourceHandle materialHandle);

      util::SingletonManager *m_singletonManager;

      bool m_inserted;

      const RenderOptions& m_options;

      util::Flags<RenderNodeType> m_nodeType;
      GLenum m_primitiveType;
      GLuint m_vertexStride;
      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;
    };
	}
}

#endif
