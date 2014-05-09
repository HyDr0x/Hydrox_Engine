#ifndef INSERTSKINNEDGEOMETRYTRAVERSER_H_
#define INSERTSKINNEDGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Traverser/InsertGeometryTraverser.h"

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"

#include "Renderer/Pipeline/RenderingOptions.h"

namespace he
{
  namespace xBar  {    class StaticGeometryContainer;
    class SkinnedGeometryContainer;  }

	namespace renderer
	{    class IRenderNode;    class InsertSkinnedGeometryTraverser : public InsertGeometryTraverser
    {
    public:

      InsertSkinnedGeometryTraverser(const xBar::SkinnedGeometryContainer& geometryContainer, const RenderOptions& soptions, util::SingletonManager *singletonManager);
      virtual ~InsertSkinnedGeometryTraverser();

      virtual bool preTraverse(IRenderNode* treeNode);

    private:

      const xBar::SkinnedGeometryContainer& m_geometryContainer;
    };
	}
}

#endif
