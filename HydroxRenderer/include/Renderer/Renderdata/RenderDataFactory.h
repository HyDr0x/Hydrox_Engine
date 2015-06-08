#ifndef RENDERDATAFACTORY_H_
#define RENDERDATAFACTORY_H_

#include <GL/glew.h>

#include <XBar/IGeometryContainer.h>

#include <Utilities/Pointer/SharedPointer.h>
#include <Utilities/Miscellaneous/Flags.hpp>

#include "Renderer/Renderdata/IRenderContainer.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class IRenderGroup;
    class RenderOptions;

    class RenderDataFactory
    {
    public:

      static util::SharedPointer<IRenderContainer> createRenderDataContainer(
        util::SharedPointer<const xBar::IGeometryContainer> geometryContainer, util::SingletonManager *singletonManager);

    private:

    };
  }
}

#endif
