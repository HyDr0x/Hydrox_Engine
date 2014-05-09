#ifndef REMOVEGEOMETRYTRAVERSER_H_
#define REMOVEGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/Traverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

namespace he
{
  namespace xBar  {    class StaticGeometryContainer;
    class SkinnedGeometryContainer;  }

	namespace renderer
	{    class IRenderNode;    class RemoveGeometryTraverser : public Traverser
    {
    public:

      RemoveGeometryTraverser(util::SingletonManager *singletonManager, const xBar::StaticGeometryContainer& geometryContainer);
      virtual ~RemoveGeometryTraverser();

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(IRenderNode* treeNode);
      virtual void postTraverse(IRenderNode* treeNode);

    private:

      void doTraverseDown(TreeNode* treeNode);

      void deleteNode(TreeNode *treeNode);

      ModelManager *m_modelManager;
	    MaterialManager *m_materialManager;

      const xBar::StaticGeometryContainer& m_geometryContainer;

      unsigned int m_vertexDeclaration;
      util::ResourceHandle m_shaderHandle;
      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;
    };
	}
}

#endif
