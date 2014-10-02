#ifndef INSERTSHADOWGEOMETRYTRAVERSER_H_
#define INSERTSHADOWGEOMETRYTRAVERSER_H_

#include <vector>

#include <DataBase/ResourceManager.hpp>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace xBar
  {
    class IGeometryContainer;
  }

  namespace renderer
  {
    class IRenderNode;

    class InsertShadowGeometryTraverser : public Traverser
    {
    public:

      InsertShadowGeometryTraverser(const xBar::IGeometryContainer& geometryContainer, const RenderOptions& options, util::SingletonManager *singletonManager);
      virtual ~InsertShadowGeometryTraverser() = 0;

      void initialize(util::ResourceHandle staticShadowMapGenerationShaderHandle, util::ResourceHandle animatedShadowMapGenerationShaderHandle);

      virtual bool preTraverse(GroupNode* treeNode);
      virtual void postTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(IRenderNode* treeNode);
      virtual void postTraverse(IRenderNode* treeNode);

      void createNewChildNode(TreeNode* parent);
      void createNewChildNode(GroupNode* parent);
      void createNewChildNode(VertexDeclarationNode* parent);
      void createNewChildNode(ShaderNode* parent);

      void createNewSibling(TreeNode* sibling);
      void createNewSibling(VertexDeclarationNode* sibling);
      void createNewSibling(ShaderNode* sibling);
      void createNewSibling(IRenderNode* sibling);

    protected:

      util::SingletonManager *m_singletonManager;

      const xBar::IGeometryContainer& m_geometryContainer;

      const RenderOptions& m_options;

      util::Flags<RenderNodeType> m_nodeType;
      GLenum m_primitiveType;
      GLuint m_vertexStride;
      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;

      bool m_inserted;
    };
  }
}

#endif
