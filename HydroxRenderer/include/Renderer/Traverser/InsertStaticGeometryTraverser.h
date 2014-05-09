#ifndef INSERTSTATICGEOMETRYTRAVERSER_H_
#define INSERTSTATICGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"

#include "Renderer/Pipeline/RenderingOptions.h"

namespace he
{
  namespace xBar
    class SkinnedGeometryContainer;

	namespace renderer
	{
    {
    public:

      InsertStaticGeometryTraverser(const xBar::StaticGeometryContainer& geometryContainer, const RenderOptions& options, util::SingletonManager *singletonManager);
      virtual ~InsertStaticGeometryTraverser();

      virtual bool preTraverse(IRenderNode* treeNode);

    private:

      const xBar::StaticGeometryContainer& m_geometryContainer;
    };
	}
}

#endif