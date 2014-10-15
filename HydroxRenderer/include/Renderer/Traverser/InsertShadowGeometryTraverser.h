#ifndef INSERTSHADOWGEOMETRYTRAVERSER_H_
#define INSERTSHADOWGEOMETRYTRAVERSER_H_

#include <vector>

#include <DataBase/ResourceManager.hpp>

#include <Utilities/Pointer/SharedPointer.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>

#include "Renderer/Traverser/Traverser.h"

namespace he
{
  namespace xBar
  {
    class IGeometryContainer;
  }

  namespace renderer
  {
    class IRenderGroup;

    class InsertShadowGeometryTraverser : public Traverser
    {
    public:

      InsertShadowGeometryTraverser(util::SharedPointer<IRenderGroup> renderGroup,
        const xBar::IGeometryContainer& geometryContainer,
        util::SingletonManager *singletonManager, 
        util::ResourceHandle staticShadowMapGenerationShaderHandle,
        util::ResourceHandle staticNormalShadowMapGenerationShaderHandle,
        util::ResourceHandle skinnedShadowMapGenerationShaderHandle,
        util::ResourceHandle skinnedNormalShadowMapGenerationShaderHandle);

      virtual ~InsertShadowGeometryTraverser();

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

      void createNewChildNode(TreeNode* parent);
      void createNewChildNode(GroupNode* parent);
      void createNewChildNode(ShaderNode* parent);
      void createNewChildNode(TextureNode* parent);
      void createNewChildNode(VertexDeclarationNode* parent);

      void createNewSibling(TreeNode* sibling);
      void createNewSibling(ShaderNode* sibling);
      void createNewSibling(TextureNode* sibling);
      void createNewSibling(VertexDeclarationNode* sibling);
      void createNewSibling(RenderNode* sibling);

    protected:

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<IRenderGroup> m_renderGroup;

      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;

      bool m_inserted;
    };
  }
}

#endif
