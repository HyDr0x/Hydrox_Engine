#ifndef INSERTGEOMETRYTRAVERSER_H_
#define INSERTGEOMETRYTRAVERSER_H_

#include <vector>

#include <DataBase/ResourceManager.hpp>

#include <Utilities/Pointer/SharedPointer.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>

#include "Renderer/Traverser/Traverser.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"

namespace he
{
  namespace xBar
  {
    class IGeometryContainer;
  }

  namespace renderer
  {
    class InsertGeometryTraverser : public Traverser
    {
    public:

      InsertGeometryTraverser(const xBar::IGeometryContainer& geometryContainer, util::SingletonManager *singletonManager);
      virtual ~InsertGeometryTraverser();

      virtual bool preTraverse(GroupNode* treeNode);
      virtual void postTraverse(GroupNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

      util::SharedPointer<IRenderGroup> getCreatedRenderNode() const;

    protected:

      void createNewChildNode(TreeNode* parent);
      void createNewChildNode(GroupNode* parent);
      void createNewChildNode(VertexDeclarationNode* parent);
      void createNewChildNode(ShaderNode* parent);
      void createNewChildNode(TextureNode* parent);

      void createNewSibling(TreeNode* sibling);
      void createNewSibling(ShaderNode* parent);
      void createNewSibling(VertexDeclarationNode* parent);
      void createNewSibling(TextureNode* sibling);
      void createNewSibling(RenderNode* sibling);

      util::SingletonManager *m_singletonManager;

      const xBar::IGeometryContainer& m_geometryContainer;

      util::SharedPointer<IRenderGroup> m_createdRenderGroup;

      util::Flags<xBar::RenderNodeType> m_nodeType;
      GLenum m_primitiveType;
      GLuint m_vertexStride;
      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;

      bool m_inserted;
    };
  }
}

#endif
