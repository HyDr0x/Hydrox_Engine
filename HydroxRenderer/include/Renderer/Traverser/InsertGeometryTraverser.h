#ifndef INSERTGEOMETRYTRAVERSER_H_
#define INSERTGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Pointer/SharedPointer.h>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/ShaderContainer.h>

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

      virtual void createNewChildNode(GroupNode* parent);
      virtual void createNewChildNode(ShaderNode* parent);
      virtual void createNewChildNode(TextureNode* parent);
      virtual void createNewChildNode(VertexDeclarationNode* parent) = 0;

      virtual void createNewSibling(ShaderNode* parent);
      virtual void createNewSibling(VertexDeclarationNode* parent);
      virtual void createNewSibling(TextureNode* sibling);
      virtual void createNewSibling(RenderNode* sibling) = 0;

      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<db::MaterialManager> m_materialManager;
      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;
      util::SharedPointer<db::ShaderContainer> m_renderShaderContainer;

      const xBar::IGeometryContainer& m_geometryContainer;

      util::Flags<xBar::RenderNodeType> m_nodeType;
      GLenum m_primitiveType;
      GLuint m_vertexStride;
      util::Flags<VertexElements> m_shaderVertexDeclaration;
      util::Flags<VertexElements> m_meshVertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;

      bool m_inserted;
    };
  }
}

#endif
