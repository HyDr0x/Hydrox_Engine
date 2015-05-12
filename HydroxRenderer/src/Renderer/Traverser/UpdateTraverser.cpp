#include "Renderer/Traverser/UpdateTraverser.h"

#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNode.h"

#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderGroup.h"

namespace he
{
  namespace renderer
  {
    UpdateTraverser::UpdateTraverser() : m_globalCacheNumber(0)
    {
    }

    UpdateTraverser::~UpdateTraverser()
    {
    }

    bool UpdateTraverser::preTraverse(RenderNode * treeNode)
    {
      treeNode->getRenderGroup()->updateBuffer();

      m_globalCacheNumber += treeNode->getRenderGroup()->getCacheNumber();

      return true;
    }

    unsigned int UpdateTraverser::getGlobalCacheNumber() const
    {
      return m_globalCacheNumber;
    }
  }
}