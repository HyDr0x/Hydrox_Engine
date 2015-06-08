#ifndef ACOMPUTEPASS_H_
#define ACOMPUTEPASS_H_

#include <list>

#include <Utilities/Math/Math.hpp>

#include <XBar/StaticGeometryContainer.h>

#include <DataBase/ResourceManager.hpp>

#include <Shader/ShaderContainer.h>

#include "Renderer/Renderpass/ShaderNode.h"

#include "Renderer/Renderdata/RenderDataFactory.h"

namespace he
{
  namespace renderer
  {
    class RenderOptions;

    class AComputepass
    {
    public:

      AComputepass();
      virtual ~AComputepass();

      virtual void initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Computepass renderPass);

      virtual void insertGeometry(util::SharedPointer<IRenderContainer> renderContainer);
      virtual void removeGeometry(util::SharedPointer<IRenderContainer> renderContainer);

      virtual void doComputepass() const = 0;

    protected:

      util::SharedPointer<RenderOptions> m_options;

      sh::ShaderContainer::Computepass m_renderPass;
      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      std::vector<util::SharedPointer<IRenderContainer>> m_renderContainer;
    };
  }
}

#endif
