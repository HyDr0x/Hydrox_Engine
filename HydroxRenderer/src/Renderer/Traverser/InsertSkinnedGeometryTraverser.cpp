#include "Renderer/Traverser/InsertSkinnedGeometryTraverser.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/TreeNodes/TreeNode.h"
#include "Renderer/TreeNodes/GroupNode.h"
#include "Renderer/TreeNodes/VertexDeclarationNode.h"
#include "Renderer/TreeNodes/ShaderNode.h"
#include "Renderer/TreeNodes/TextureNode.h"
#include "Renderer/TreeNodes/RenderNodeDecorator/IRenderNode.h"

#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    InsertSkinnedGeometryTraverser::InsertSkinnedGeometryTraverser(const xBar::SkinnedGeometryContainer& geometryContainer, const RenderOptions& options, util::SingletonManager *singletonManager) : 
      InsertGeometryTraverser(options, singletonManager),
      m_geometryContainer(geometryContainer)
    {
      Mesh* mesh = m_singletonManager->getService<ModelManager>()->getObject(m_geometryContainer.getMeshHandle());

      if(mesh->getIndexCount() > 0)
      {
        m_nodeType = util::Flags<RenderNodeType>(SKINNEDNODE | INDEXEDNODE);
      }
      else
      {
        m_nodeType = util::Flags<RenderNodeType>(SKINNEDNODE);
      }

      initialize(mesh, m_geometryContainer.getMaterialHandle());
    }

    InsertSkinnedGeometryTraverser::~InsertSkinnedGeometryTraverser()
    {
    }

    bool InsertSkinnedGeometryTraverser::preTraverse(IRenderNode* treeNode)
    {
      m_inserted = false;
      if(treeNode->insertGeometry(m_geometryContainer))
      {
        m_stopTraversal = true;
        m_inserted = true;
      }

      return false;
    }
	}
}