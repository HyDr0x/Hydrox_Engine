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

    void BillboardRenderer::initialize(util::SingletonManager *singletonManager, util::ResourceHandle billboardShaderHandle)
    {
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();
      m_textureManager = singletonManager->getService<TextureManager>();

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_billboardShaderHandle = billboardShaderHandle;

      glGenBuffers(1, &m_dummyVBO);
      glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
      glVertexAttribPointer(RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void BillboardRenderer::render()
    {
      Texture2D *renderTexture;

	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glEnableVertexAttribArray(RenderShader::SPECIAL0);

      glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
 
      RenderShader *billboardShader = m_renderShaderManager->getObject(m_billboardShaderHandle);
	    billboardShader->useShader();

	    for(std::list<xBar::BillboardContainer>::iterator billboarditerator = m_renderBillboards.begin(); billboarditerator != m_renderBillboards.end(); billboarditerator++)
	    {
        renderTexture = m_textureManager->getObject((*billboarditerator).getTextureHandle());

        renderTexture->setTexture(6, 0);

		    util::Matrix<float, 3> tmpTexTrfMatrix = (*billboarditerator).getTexTransformationMatrix();
        util::Vector<float, 2> scale = (*billboarditerator).getScale();
		    util::Vector<float, 3> translate = (*billboarditerator).getPosition();
		    billboardShader->setUniform(3, GL_FLOAT_MAT3, &tmpTexTrfMatrix[0][0]);
		    billboardShader->setUniform(4, GL_FLOAT_VEC2, &scale[0]);
		    billboardShader->setUniform(5, GL_FLOAT_VEC3, &translate[0]);

        glDrawArrays(GL_POINTS, 0, 1);
	    }
    
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(RenderShader::SPECIAL0);

      glDisable(GL_BLEND);
    }

    void BillboardRenderer::addRenderComponent(xBar::BillboardContainer& billboard)
    {
      m_renderBillboards.push_back(billboard);
    }

    void BillboardRenderer::removeRenderComponent(xBar::BillboardContainer& billboard)
    {
      m_renderBillboards.remove(billboard);
    }

    void BillboardRenderer::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(xBar::BillboardContainer& billboard)>(util::EventManager::OnAddBillboardNode);
      eventManager->addSlotToSignal<BillboardRenderer, void (*)(xBar::BillboardContainer& billboard), void (BillboardRenderer::*)(xBar::BillboardContainer& billboard)>(this, &BillboardRenderer::addRenderComponent, util::EventManager::OnAddBillboardNode);

      eventManager->addNewSignal<void (*)(xBar::BillboardContainer& billboard)>(util::EventManager::OnRemoveBillboardNode);
      eventManager->addSlotToSignal<BillboardRenderer, void (*)(xBar::BillboardContainer& billboard), void (BillboardRenderer::*)(xBar::BillboardContainer& billboard)>(this, &BillboardRenderer::removeRenderComponent, util::EventManager::OnRemoveBillboardNode);
    }
	}
}
