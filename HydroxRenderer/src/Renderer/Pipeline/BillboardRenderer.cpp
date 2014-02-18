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
      Texture *renderTexture;

	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glEnableVertexAttribArray(RenderShader::SPECIAL0);
 
      RenderShader *billboardShader = m_renderShaderManager->getObject(m_billboardShaderHandle);
	    billboardShader->useShader();

	    for(std::list<sg::BillboardNode*>::const_iterator billboarditerator = m_renderBillboards.begin(); billboarditerator != m_renderBillboards.end(); billboarditerator++)
	    {
        if((*billboarditerator)->getRenderable())
        {
          renderTexture = m_textureManager->getObject((*billboarditerator)->getTextureHandle());

          renderTexture->setTexture(6, 0);

          util::Matrix<float, 4> worldMatrix = (*billboarditerator)->getTransformationMatrix();
		      util::Matrix<float, 3> tmpTexTrfMatrix = (*billboarditerator)->getTexTransformationMatrix();
          util::Vector<float, 2> scale = (*billboarditerator)->getScale();
		      util::Vector<float, 3> translate = (*billboarditerator)->getPosition();
          billboardShader->setUniform(0, GL_FLOAT_MAT4, &worldMatrix[0][0]);
		      billboardShader->setUniform(3, GL_FLOAT_MAT3, &tmpTexTrfMatrix[0][0]);
		      billboardShader->setUniform(4, GL_FLOAT_VEC2, &scale[0]);
		      billboardShader->setUniform(5, GL_FLOAT_VEC3, &translate[0]);

          glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
          glDrawArrays(GL_POINTS, 0, 1);
        }
	    }
    
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(RenderShader::SPECIAL0);

      glDisable(GL_BLEND);
    }

    void BillboardRenderer::addRenderComponent(sg::BillboardNode *billboardNode)
    {
      m_renderBillboards.push_back(billboardNode);
    }

    void BillboardRenderer::removeRenderComponent(sg::BillboardNode *billboardNode)
    {
      m_renderBillboards.remove(billboardNode);
    }

    void BillboardRenderer::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(sg::BillboardNode *node)>(util::EventManager::OnAddBillboardNode);
      eventManager->addSlotToSignal<BillboardRenderer, void (*)(sg::BillboardNode *node), void (BillboardRenderer::*)(sg::BillboardNode *node)>(this, &BillboardRenderer::addRenderComponent, util::EventManager::OnAddBillboardNode);

      eventManager->addNewSignal<void (*)(sg::BillboardNode *node)>(util::EventManager::OnRemoveBillboardNode);
      eventManager->addSlotToSignal<BillboardRenderer, void (*)(sg::BillboardNode *node), void (BillboardRenderer::*)(sg::BillboardNode *node)>(this, &BillboardRenderer::removeRenderComponent, util::EventManager::OnRemoveBillboardNode);
    }
	}
}
