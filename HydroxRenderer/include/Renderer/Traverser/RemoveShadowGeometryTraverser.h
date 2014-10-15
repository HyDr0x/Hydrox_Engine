#ifndef REMOVESHADOWGEOMETRYTRAVERSER_H_
#define REMOVESHADOWGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace xBar  {    class IGeometryContainer;  }

  namespace renderer
  {    class RemoveShadowGeometryTraverser : public Traverser
    {
    public:

      RemoveShadowGeometryTraverser(
        util::SingletonManager *singletonManager, 
        const xBar::IGeometryContainer& geometryContainer, 
        util::ResourceHandle staticShadowMapGenerationShaderHandle,
        util::ResourceHandle staticNormalShadowMapGenerationShaderHandle,
        util::ResourceHandle skinnedShadowMapGenerationShaderHandle,
        util::ResourceHandle skinnedNormalShadowMapGenerationShaderHandle);

      virtual ~RemoveShadowGeometryTraverser();

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(RenderNode* treeNode);
      virtual void postTraverse(RenderNode* treeNode);

    private:

      void doTraverseDown(TreeNode* treeNode);

      void deleteNode(TreeNode *treeNode);

      db::ModelManager *m_modelManager;
      db::MaterialManager *m_materialManager;

      const xBar::IGeometryContainer& m_geometryContainer;

      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;
    };
  }
}

#endif
