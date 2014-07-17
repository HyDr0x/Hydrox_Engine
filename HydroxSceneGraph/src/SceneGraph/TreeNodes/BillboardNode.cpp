#include "SceneGraph/TreeNodes/BillboardNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
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
                                 const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) :
    TreeNode(nodeName, parent, nextSibling),
    m_eventManager(eventManager),
    m_textureHandle(textureHandle),
    m_animNumber(animNumber), 
    m_texStart(texStart), 
    m_texEnd(texEnd),
    m_translate(util::Vector<float, 3>::identity()),
    m_scale(util::Vector<float, 2>(1.0f, 1.0f)),
    m_renderable(false)
    {
      m_index.nodeType = BILLBOARDNODE;
    }

    BillboardNode::BillboardNode(const TreeNode& sourceNode) : TreeNode(sourceNode.getNodeName(), sourceNode.getParent(), sourceNode.getNextSibling())
    {
      assert(BILLBOARDNODE == sourceNode.getNodeType());

      const BillboardNode& copyNode = static_cast<const BillboardNode&>(sourceNode);

      new (this) BillboardNode(copyNode);
    }

    BillboardNode::~BillboardNode()
    {
    }

    TreeNode& BillboardNode::operator=(const TreeNode& other)
    {
      assert(m_index.nodeType == other.getNodeType());

      this->~BillboardNode();

      new (this) BillboardNode(other);

      return *this;
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
      return traverser->ascendTraverse(*this);
    }

    bool BillboardNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void BillboardNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool BillboardNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool BillboardNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void BillboardNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
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

    void BillboardNode::setAnimation(const util::Vector<unsigned int, 2>& number)
    {
      assert(number[1] < m_animNumber[1] && number[0] < m_animNumber[0]);
      m_animCount = number;
    }

    util::Vector<unsigned int, 2> BillboardNode::getAnimationNumber() const
    {
      return m_animNumber;
    }

    util::Vector<unsigned int, 2> BillboardNode::getAnimationCount() const
    {
      return m_animCount;
    }

    void BillboardNode::setTranslation(float x, float y, float z)
    {
      m_translate[0] = x;
      m_translate[1] = y;
      m_translate[2] = z;
    }

    void BillboardNode::setTranslation(const util::Vector<float, 3>& v)
    {
      m_translate = v;
    }

    void BillboardNode::addTranslation(float x, float y, float z)
    {
      m_translate[0] += x;
      m_translate[1] += y;
      m_translate[2] += z;
    }

    void BillboardNode::addTranslation(const util::Vector<float, 3>& v)
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

    void BillboardNode::setScale(const util::Vector<float, 2>& s)
    {
      m_scale = s;
    }

    void BillboardNode::setScale(float sx, float sy)
    {
      m_scale[0] = sx;
      m_scale[1] = sy;
    }

    void BillboardNode::addScale(const util::Vector<float, 2>& s)
    {
      m_scale = s;
    }

    void BillboardNode::addScale(float sx, float sy)
    {
      m_scale[0] += sx;
      m_scale[1] += sy;
    }

    util::Vector<float, 3> BillboardNode::getPosition() const
    {
      return m_translate;
    }

    util::Vector<float, 2> BillboardNode::getScale() const
    {
      return m_scale;
    }

    util::Vector<float, 2> BillboardNode::getTextureStart() const
    {
      return m_texStart;
    }

    util::Vector<float, 2> BillboardNode::getTextureEnd() const
    {
      return m_texEnd;
    }

    util::Matrix<float, 3> BillboardNode::getTexTransformationMatrix() const
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
        m_eventManager->raiseSignal<void (*)(const xBar::BillboardContainer& billboard)>(util::EventManager::OnAddBillboardNode)->execute(xBar::BillboardContainer(m_translate, m_scale, m_animNumber, m_animCount, m_texStart, m_texEnd, m_textureHandle));
      }
      else if(m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void (*)(const xBar::BillboardContainer& billboard)>(util::EventManager::OnRemoveBillboardNode)->execute(xBar::BillboardContainer(m_translate, m_scale, m_animNumber, m_animCount, m_texStart, m_texEnd, m_textureHandle));
      }

      m_renderable = renderable;
    }

    bool BillboardNode::getRenderable() const
    {
      return m_renderable;
    }
  }
}