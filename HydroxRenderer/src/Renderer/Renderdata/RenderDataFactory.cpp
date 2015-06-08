#include "Renderer/Renderdata/RenderDataFactory.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/Renderdata/RenderContainer.h"

#include "Renderer/Renderdata/MaterialRenderData.h"
#include "Renderer/Renderdata/DrawArrayRenderData.h"
#include "Renderer/Renderdata/DrawElementsRenderData.h"
#include "Renderer/Renderdata/SkinnedInstancesRenderData.h"
#include "Renderer/Renderdata/SkinnedMatrixRenderData.h"
#include "Renderer/Renderdata/StaticInstancesRenderData.h"
#include "Renderer/Renderdata/StaticMatrixRenderData.h"

namespace he
{
  namespace renderer
  {
    util::SharedPointer<IRenderContainer> RenderDataFactory::createRenderDataContainer(
      util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager
      )
    {
      util::SharedPointer<RenderOptions> options = singletonManager->getService<RenderOptions>();
      util::SharedPointer<db::ModelManager> modelManager = singletonManager->getService<db::ModelManager>();
      util::SharedPointer<db::MaterialManager> materialManager = singletonManager->getService<db::MaterialManager>();

      db::Mesh *mesh = modelManager->getObject(geometryContainer->getMeshHandle());
      GLenum primitiveType = mesh->getPrimitiveType();
      unsigned int vertexStride = mesh->getVertexStride();
      VertexElementFlags meshVertexDeclaration = mesh->getVertexDeclarationFlags();

      util::Flags<xBar::RenderNodeType> nodeType = geometryContainer->getNodeType();

      if(mesh->getIndexCount())
      {
        nodeType |= util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE);
      }
      else
      {
        nodeType |= util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::NONINDEXEDNODE);
      }

      std::vector<std::vector<util::ResourceHandle>> textureHandles = materialManager->getObject(geometryContainer->getMaterialHandle())->getTextureHandles();

      if((util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::SKINNEDNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE)) == nodeType)
      {
        return util::SharedPointer<IRenderContainer>(
          new RenderContainer<MaterialRenderData, DrawElementsRenderData, SkinnedMatrixRenderData, SkinnedInstancesRenderData>(
          MaterialRenderData(singletonManager, textureHandles), DrawElementsRenderData(GLINDEXTYPE, primitiveType, vertexStride, meshVertexDeclaration, singletonManager), SkinnedMatrixRenderData(options), SkinnedInstancesRenderData(options)
          ));
      }
      else if((util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::INDEXEDNODE)) == nodeType)
      {
        return util::SharedPointer<IRenderContainer>(
          new RenderContainer<MaterialRenderData, DrawElementsRenderData, StaticMatrixRenderData, StaticInstancesRenderData>(
          MaterialRenderData(singletonManager, textureHandles), DrawElementsRenderData(GLINDEXTYPE, primitiveType, vertexStride, meshVertexDeclaration, singletonManager), StaticMatrixRenderData(options), StaticInstancesRenderData(options)
          ));
      }
      else if((util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::SKINNEDNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::NONINDEXEDNODE)) == nodeType)
      {
        return util::SharedPointer<IRenderContainer>(
          new RenderContainer<MaterialRenderData, DrawArrayRenderData, SkinnedMatrixRenderData, SkinnedInstancesRenderData>(
          MaterialRenderData(singletonManager, textureHandles), DrawArrayRenderData(primitiveType, vertexStride, meshVertexDeclaration, singletonManager), SkinnedMatrixRenderData(options), SkinnedInstancesRenderData(options)
          ));
      }
      else //if((util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE) | util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::NONINDEXEDNODE)) == nodeType)
      {
        return util::SharedPointer<IRenderContainer>(
          new RenderContainer<MaterialRenderData, DrawArrayRenderData, StaticMatrixRenderData, StaticInstancesRenderData>(
          MaterialRenderData(singletonManager, textureHandles), DrawArrayRenderData(primitiveType, vertexStride, meshVertexDeclaration, singletonManager), StaticMatrixRenderData(options), StaticInstancesRenderData(options)
          ));
      }
    }
  }
}