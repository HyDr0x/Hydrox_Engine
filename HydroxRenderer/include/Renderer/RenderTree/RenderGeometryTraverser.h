#ifndef RENDERGEOMETRYTRAVERSER_H_
#define RENDERGEOMETRYTRAVERSER_H_

#include <vector>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/RenderTree/Traverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

namespace he
{
	namespace renderer
	{    class RenderGeometryTraverser : public Traverser
    {
    public:

      RenderGeometryTraverser(ModelManager *modelManager, MaterialManager *materialManager, RenderShaderManager *renderShaderManager, TextureManager *textureManager);
      virtual ~RenderGeometryTraverser();

      virtual bool preTraverse(VertexDeclarationNode* treeNode);
      virtual void postTraverse(VertexDeclarationNode* treeNode);

      virtual bool preTraverse(ShaderNode* treeNode);
      virtual void postTraverse(ShaderNode* treeNode);

      virtual bool preTraverse(TextureNode* treeNode);
      virtual void postTraverse(TextureNode* treeNode);

      virtual bool preTraverse(StaticRenderNode* treeNode);
      virtual void postTraverse(StaticRenderNode* treeNode);

      virtual bool preTraverse(AnimatedRenderNode* treeNode);
      virtual void postTraverse(AnimatedRenderNode* treeNode);

    private:

      ModelManager *m_modelManager;
	    MaterialManager *m_materialManager;
	    RenderShaderManager *m_renderShaderManager;
      TextureManager *m_textureManager;
    };
	}
}

#endif
