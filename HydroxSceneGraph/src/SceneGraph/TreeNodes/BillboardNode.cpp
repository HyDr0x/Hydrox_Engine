#include "SceneGraph/TreeNodes/BillboardNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

#include <XBar/BillboardContainer.h>

namespace he
{
	namespace sg
	{
    BillboardNode::BillboardNode(util::EventManager *eventManager,
                                util::ResourceHandle textureHandle,
                                util::Vector<unsigned int, 2> animNumber, 
                                util::Vector<float, 2> texStart, 
                                util::Vector<float, 2> texEnd, 
                                const std::string& nodeName, 
                                GroupNode* parent, 
                                TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling),
                                                         m_eventManager(eventManager),
                                                         m_textureHandle(textureHandle),
                                                         m_animNumber(animNumber), 
                                                         m_texStart(texStart), 
                                                         m_texEnd(texEnd),
                                                         m_translate(util::Vector<float, 3>::identity()),
                                                         m_scale(util::Vector<float, 2>(1.0f, 1.0f)),
                                                         m_renderable(false)
    {
    }

    BillboardNode& BillboardNode::operator=(const BillboardNode& o)
    {
      TreeNode::operator=(o);

      m_eventManager = o.m_eventManager;

      m_textureHandle = o.m_textureHandle;
      m_renderable = o.m_renderable;

      m_animNumber = o.m_animNumber;
	    m_animCount = o.m_animCount;
	    m_texStart = o.m_texStart;
	    m_texEnd = o.m_texEnd;

	    m_scale = o.m_scale;
	    m_translate = o.m_translate;

      return *this;
    }

    TreeNode& BillboardNode::operator=(const TreeNode& o)
    {
      assert(typeid(*this) == typeid(o));

      const BillboardNode& copyNode = static_cast<const BillboardNode&>(o);
      BillboardNode::operator=(copyNode);

      return *this;
    }

    BillboardNode::~BillboardNode()
    {
    }

    TreeNode* BillboardNode::clone() const
    {
      BillboardNode *newNode = new BillboardNode(m_eventManager, m_textureHandle, m_animNumber, m_texStart, m_texEnd, m_nodeName);

	    newNode->m_scale = m_scale;
	    newNode->m_translate = m_translate;

      return newNode;
    }

    bool BillboardNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(this);
    }

    bool BillboardNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void BillboardNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool BillboardNode::isBillboardNode()
    {
      return true;
    }

    util::ResourceHandle BillboardNode::getTextureHandle() const
    {
      return m_textureHandle;
    }

    void BillboardNode::setAnimation(unsigned int number)
    {
      assert(number / m_animNumber[0] < m_animNumber[1]);
      m_animCount = util::Vector<unsigned int, 2>(number % m_animNumber[0], number / m_animNumber[0]);
    }

    void BillboardNode::setAnimation(util::Vector<unsigned int, 2> number)
    {
      assert(number[1] < m_animNumber[1] && number[0] < m_animNumber[0]);
      m_animCount = number;
    }

    util::Vector<unsigned int, 2> BillboardNode::getAnimationNumber()
    {
      return m_animNumber;
    }

    util::Vector<unsigned int, 2> BillboardNode::getAnimationCount()
    {
      return m_animCount;
    }

    void BillboardNode::setTranslation(float x, float y, float z)
    {
	    m_translate[0] = x;
	    m_translate[1] = y;
	    m_translate[2] = z;
    }

    void BillboardNode::setTranslation(util::Vector<float, 3> v)
    {
	    m_translate = v;
    }

    void BillboardNode::addTranslation(float x, float y, float z)
    {
	    m_translate[0] += x;
	    m_translate[1] += y;
	    m_translate[2] += z;
    }

    void BillboardNode::addTranslation(util::Vector<float, 3> v)
    {
	    m_translate = v;
    }

    void BillboardNode::setScale(float s)
    {
	    m_scale[0] = s;
	    m_scale[1] = s;
    }

    void BillboardNode::addScale(float s)
    {
	    m_scale[0] += s;
	    m_scale[1] += s;
    }

    void BillboardNode::setScale(util::Vector<float, 2> s)
    {
	    m_scale = s;
    }

    void BillboardNode::setScale(float sx, float sy)
    {
	    m_scale[0] = sx;
	    m_scale[1] = sy;
    }

    void BillboardNode::addScale(util::Vector<float, 2> s)
    {
	    m_scale = s;
    }

    void BillboardNode::addScale(float sx, float sy)
    {
	    m_scale[0] += sx;
	    m_scale[1] += sy;
    }

    util::Vector<float, 3> BillboardNode::getPosition()
    {
	    return m_translate;
    }

    util::Vector<float, 2> BillboardNode::getScale()
    {
	    return m_scale;
    }

    util::Matrix<float, 3> BillboardNode::getTexTransformationMatrix()
    {
	    float width  = m_texEnd[0] - m_texStart[0];
	    float height = m_texEnd[1] - m_texStart[1];

      return util::Matrix<float,3>(width/m_animNumber[0],0.0f,(static_cast<float>(m_animCount[0])/static_cast<float>(m_animNumber[0]))*width+m_texStart[0], 
					      0.0f,height/m_animNumber[1],(static_cast<float>(m_animCount[1])/static_cast<float>(m_animNumber[1]))*height+m_texStart[1], 
					      0.0f,0.0f,1.0f);
    }

    void BillboardNode::setRenderable(bool renderable)
    {
      if(!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::BillboardContainer& billboard)>(util::EventManager::OnAddBillboardNode)->execute(xBar::BillboardContainer(m_translate, m_scale, m_animNumber, m_animCount, m_texStart, m_texEnd, m_textureHandle));
      }
      else if(m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::BillboardContainer& billboard)>(util::EventManager::OnRemoveBillboardNode)->execute(xBar::BillboardContainer(m_translate, m_scale, m_animNumber, m_animCount, m_texStart, m_texEnd, m_textureHandle));
      }

      m_renderable = renderable;
    }

    bool BillboardNode::getRenderable() const
    {
      return m_renderable;
    }
	}
}