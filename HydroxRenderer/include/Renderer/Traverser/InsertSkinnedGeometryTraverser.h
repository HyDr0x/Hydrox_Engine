#ifndef INSERTSKINNEDGEOMETRYTRAVERSER_H_
#define INSERTSKINNEDGEOMETRYTRAVERSER_H_

#include <vector>

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/TreeNodes/RenderNodeDecorator/RenderNodeFactory.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace xBar
  {
    class SkinnedGeometryContainer;
  }

  namespace renderer
  {
    class IRenderNode;

    template<typename InsertTraverser> class InsertSkinnedGeometryTraverser : public InsertTraverser
    {
    public:

      InsertSkinnedGeometryTraverser(const xBar::SkinnedGeometryContainer& geometryContainer, const RenderOptions& soptions, util::SingletonManager *singletonManager);
      virtual ~InsertSkinnedGeometryTraverser();
    };
  }
}

#include "Renderer/Traverser/InsertSkinnedGeometryTraverser.inl"

#endif
