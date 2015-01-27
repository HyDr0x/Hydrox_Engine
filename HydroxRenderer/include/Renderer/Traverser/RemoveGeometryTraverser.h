#ifndef REMOVEGEOMETRYTRAVERSER_H_
#define REMOVEGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ShaderContainer.h>
#include <DataBase/ResourceManager.hpp>

#include "Renderer/Traverser/Traverser.h"

namespace he
{
  namespace xBar  {    class IGeometryContainer;  }

  namespace renderer
  {    class RemoveGeometryTraverser : public Traverser
    {
    public:

      RemoveGeometryTraverser(util::SingletonManager *singletonManager, const xBar::IGeometryContainer& geometryContainer);
      virtual ~RemoveGeometryTraverser();

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode) = 0;

    protected:

      void doTraverseDown(TreeNode* treeNode);

      void deleteNode(TreeNode *treeNode);

      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<db::MaterialManager> m_materialManager;
      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;
      util::SharedPointer<db::ShaderContainer> m_renderShaderContainer;

      const xBar::IGeometryContainer& m_geometryContainer;

      util::Flags<db::VertexDeclarationFlags> m_shaderVertexDeclaration;
      util::Flags<db::VertexDeclarationFlags> m_meshVertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;
    };
  }
}

#endif
