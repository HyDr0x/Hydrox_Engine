#include "Renderer/RenderTree/DeleteTraverser.h"

#include "Renderer/RenderTree/VertexDeclarationNode.h"
#include "Renderer/RenderTree/ShaderNode.h"
#include "Renderer/RenderTree/TextureNode.h"
#include "Renderer/RenderTree/StaticRenderNode.h"
#include "Renderer/RenderTree/AnimatedRenderNode.h"

namespace he
{
	namespace renderer
	{
    DeleteTraverser::DeleteTraverser()
    {
    }

    DeleteTraverser::~DeleteTraverser()
    {
    }

    bool DeleteTraverser::preTraverse(VertexDeclarationNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(VertexDeclarationNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(ShaderNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(ShaderNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(TextureNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(TextureNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(AnimatedRenderNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(AnimatedRenderNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(StaticRenderNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(StaticRenderNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }
	}
}