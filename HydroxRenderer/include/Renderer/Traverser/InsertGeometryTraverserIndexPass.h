#ifndef INSERTGEOMETRYTRAVERSERINDEXPASS_H_
#define INSERTGEOMETRYTRAVERSERINDEXPASS_H_

#include "Renderer/Traverser/InsertGeometryTraverser.h"

namespace he
{
  namespace renderer
  {
    class InsertGeometryTraverserIndexPass : public InsertGeometryTraverser
    {
    public:

      InsertGeometryTraverserIndexPass(
        util::SharedPointer<IRenderGroup> sharedRenderGroup,
        const xBar::IGeometryContainer& geometryContainer, 
        util::SingletonManager *singletonManager, 
        util::ResourceHandle staticIndexGenerationShaderHandle,
        util::ResourceHandle skinnedIndexGenerationShaderHandle);

      virtual ~InsertGeometryTraverserIndexPass();

      virtual bool preTraverse(RenderNode* treeNode);

    protected:

      void createNewChildNode(VertexDeclarationNode* parent);
      void createNewSibling(RenderNode* sibling);

      util::SharedPointer<IRenderGroup> m_sharedRenderGroup;
    };
  }
}

#endif
