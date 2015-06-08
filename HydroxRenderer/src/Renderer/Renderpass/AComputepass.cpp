#include "Renderer/Renderpass/AComputepass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    AComputepass::AComputepass()
    {
      
    }

    AComputepass::~AComputepass()
    {
    }

    void AComputepass::initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Computepass renderPass)
    {
      m_options = singletonManager->getService<RenderOptions>();
      m_renderPass = renderPass;
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();
    }

    void AComputepass::insertGeometry(util::SharedPointer<IRenderContainer> renderContainer)
    {
      m_renderContainer.push_back(renderContainer);
    }

    void AComputepass::removeGeometry(util::SharedPointer<IRenderContainer> renderContainer)
    {
      std::vector<util::SharedPointer<IRenderContainer>>::iterator it = std::find(m_renderContainer.begin(), m_renderContainer.end(), renderContainer);
      m_renderContainer.erase(it);
    }
  }
}
