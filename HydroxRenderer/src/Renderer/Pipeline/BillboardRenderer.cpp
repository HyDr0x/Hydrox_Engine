#include "Renderer/Pipeline/BillboardRenderer.h"

namespace he
{
  namespace renderer
  {
    BillboardRenderer::BillboardRenderer()
    {
    }

    BillboardRenderer::~BillboardRenderer()
    {
      glDeleteBuffers(1, &m_dummyVBO);

      m_renderBillboards.clear();
    }

    void BillboardRenderer::initialize(util::SingletonManager *singletonManager)
    {
      m_renderShaderContainer = singletonManager->getService<sh::ShaderContainer>();
      m_textureManager = singletonManager->getService<db::TextureManager>();

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_billboardShaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::BILLBOARD, sh::ShaderSlotFlags(8192));

      glGenBuffers(1, &m_dummyVBO);
      glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
      glVertexAttribPointer(sh::RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void BillboardRenderer::render() const
    {
      db::Texture2D *renderTexture;

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glEnableVertexAttribArray(sh::RenderShader::SPECIAL0);

      glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
 
      const sh::RenderShader& billboardShader = m_renderShaderContainer->getRenderShader(m_billboardShaderHandle);
      billboardShader.useShader();

      for(std::list<xBar::BillboardContainer>::const_iterator billboarditerator = m_renderBillboards.begin(); billboarditerator != m_renderBillboards.end(); billboarditerator++)
      {
        renderTexture = m_textureManager->getObject((*billboarditerator).getTextureHandle());

        renderTexture->setTexture(6, 0);

        util::Matrix<float, 3> tmpTexTrfMatrix = (*billboarditerator).getTexTransformationMatrix();
        util::vec2f scale = (*billboarditerator).getScale();
        util::vec3f translate = (*billboarditerator).getPosition();
        sh::RenderShader::setUniform(3, GL_FLOAT_MAT3, &tmpTexTrfMatrix[0][0]);
        sh::RenderShader::setUniform(4, GL_FLOAT_VEC2, &scale[0]);
        sh::RenderShader::setUniform(5, GL_FLOAT_VEC3, &translate[0]);

        glDrawArrays(GL_POINTS, 0, 1);
      }
    
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(sh::RenderShader::SPECIAL0);

      glDisable(GL_BLEND);
    }

    void BillboardRenderer::addRenderComponent(const xBar::BillboardContainer& billboard)
    {
      m_renderBillboards.push_back(billboard);
    }

    void BillboardRenderer::removeRenderComponent(const xBar::BillboardContainer& billboard)
    {
      m_renderBillboards.remove(billboard);
    }

    void BillboardRenderer::registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager)
    {
      eventManager->addNewSignal<void (*)(const xBar::BillboardContainer& billboard)>(util::EventManager::OnAddBillboardNode);
      eventManager->addSlotToSignal<BillboardRenderer, void (*)(const xBar::BillboardContainer& billboard), void (BillboardRenderer::*)(const xBar::BillboardContainer& billboard)>(this, &BillboardRenderer::addRenderComponent, util::EventManager::OnAddBillboardNode);

      eventManager->addNewSignal<void (*)(const xBar::BillboardContainer& billboard)>(util::EventManager::OnRemoveBillboardNode);
      eventManager->addSlotToSignal<BillboardRenderer, void (*)(const xBar::BillboardContainer& billboard), void (BillboardRenderer::*)(const xBar::BillboardContainer& billboard)>(this, &BillboardRenderer::removeRenderComponent, util::EventManager::OnRemoveBillboardNode);
    }
  }
}
