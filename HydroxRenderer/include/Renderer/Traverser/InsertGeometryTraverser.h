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

      virtual bool preTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);

    protected:

      void createNewChildNode(GroupNode* parent);
      void createNewChildNode(ShaderNode* parent);
      void createNewChildNode(TextureNode* parent);
      virtual void createNewChildNode(VertexDeclarationNode* parent) = 0;

      void createNewSibling(ShaderNode* parent);
      void createNewSibling(VertexDeclarationNode* parent);
      void createNewSibling(TextureNode* sibling);
      virtual void createNewSibling(RenderNode* sibling) = 0;

      util::SingletonManager *m_singletonManager;

      const xBar::IGeometryContainer& m_geometryContainer;

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
