#ifndef MATERIALDECORATOR_H_
#define MATERIALDECORATOR_H_

#include <utility>

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

namespace he
{
	namespace renderer
	{
    class MaterialDecorator : public ARenderNodeDecorator
    {
    public:

      MaterialDecorator(IRenderNode *renderNode, unsigned int maxMaterials, util::SingletonManager *singletonManager);
      ~MaterialDecorator();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual void rasterizeGeometry();

      virtual void updateBuffer();

    protected:

      void resizeBuffer();

      MaterialManager *m_materialManager;

      UBO m_materialBuffer;
      GPUBuffer m_materialIndexBuffer;

      unsigned int m_maxMaterials;
      unsigned int m_materialCount;

      std::map<util::ResourceHandle, unsigned int, Less> m_materialHandles;

      typedef std::map<util::ResourceHandle, std::list<std::pair<uint64_t, unsigned int>>, Less> MeshMaterialHandles;
      MeshMaterialHandles m_meshHandles;
    };
  }
}

#endif