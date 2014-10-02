#ifndef INSERTSTATICGEOMETRYTRAVERSER_H_
#define INSERTSTATICGEOMETRYTRAVERSER_H_

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
    class StaticGeometryContainer;
  }

  namespace renderer
  {
    class IRenderNode;

    template<typename InsertTraverser> class InsertStaticGeometryTraverser : public InsertTraverser
    {
    public:

      InsertStaticGeometryTraverser(const xBar::StaticGeometryContainer& geometryContainer, const RenderOptions& options, util::SingletonManager *singletonManager);
      virtual ~InsertStaticGeometryTraverser();
    };
  }
}

#include "Renderer/Traverser/InsertStaticGeometryTraverser.inl"

#endif
