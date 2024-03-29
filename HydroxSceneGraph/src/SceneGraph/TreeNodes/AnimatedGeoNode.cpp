#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
	namespace sg
	{
    AnimatedGeoNode::AnimatedGeoNode(const std::vector<util::Matrix<float, 4>>& inverseBindPoseMatrices, util::EventManager *eventManager, util::ResourceHandle meshHandle, util::ResourceHandle materialHandle, bool transparency, bool instanced, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) 
      : GeoNode(eventManager, meshHandle, materialHandle, transparency, instanced, nodeName, parent, nextSibling), m_inverseBindPoseMatrices(inverseBindPoseMatrices)
    {
      m_boneTransformMatrices.resize(m_inverseBindPoseMatrices.size());
    }

    AnimatedGeoNode& AnimatedGeoNode::operator=(const AnimatedGeoNode& sourceNode)
    {
      GeoNode::operator=(sourceNode);

      m_inverseBindPoseMatrices = sourceNode.m_inverseBindPoseMatrices;
      m_boneTransformMatrices = sourceNode.m_boneTransformMatrices;

      return *this;
    }

    TreeNode& AnimatedGeoNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const AnimatedGeoNode& copyNode = static_cast<const AnimatedGeoNode&>(sourceNode);
      AnimatedGeoNode::operator=(copyNode);

      return *this;
    }

    AnimatedGeoNode::~AnimatedGeoNode()
    {
    }

    TreeNode* AnimatedGeoNode::clone() const
    {
      AnimatedGeoNode *newNode = new AnimatedGeoNode(m_inverseBindPoseMatrices, m_eventManager, m_meshHandle, m_materialHandle, m_transparency, m_instanced, m_nodeName);

      newNode->m_trfMatrix = m_trfMatrix;

      newNode->m_boneTransformMatrices = m_boneTransformMatrices;

      return newNode;
    }

    bool AnimatedGeoNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(this);
    }

    bool AnimatedGeoNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void AnimatedGeoNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool AnimatedGeoNode::isAnimatedGeoNode()
    {
      return true;
    }

    void AnimatedGeoNode::setMeshHandle(util::ResourceHandle meshHandle)
    {
      if(m_renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnRemoveSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
      }

      m_meshHandle = meshHandle;

      if(m_renderable)
      {
        if(m_instanced)
        {
          m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnAddInstancedSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
        }
        else
        {
          m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnAddSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
        }
      }
    }

    void AnimatedGeoNode::setMaterialHandle(util::ResourceHandle materialHandle)
    {
      if(m_renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnRemoveSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
      }

      m_materialHandle = materialHandle;

      if(m_renderable)
      {
        if(m_instanced)
        {
          m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnAddInstancedSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
        }
        else
        {
          m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnAddSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
        }
      }
    }

    void AnimatedGeoNode::setRenderable(bool renderable)
    {
      if(!m_renderable && renderable)
      {
        if(m_instanced)
        {
          m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnAddInstancedSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
        }
        else
        {
          m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnAddSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
        }
      }
      else if(m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::SkinnedGeometryContainer& skinnedGeometry)>(util::EventManager::OnRemoveSkinnedGeometryNode)->execute(xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, &m_trfMatrix, m_materialHandle, m_meshHandle));
      }

      m_renderable = renderable;
    }

    void AnimatedGeoNode::setBoneTransform(const util::Matrix<float, 4>& boneTransform, unsigned int boneIndex)
    {
      m_boneTransformMatrices[boneIndex] = boneTransform;
    }

    std::vector<util::Matrix<float, 4>> AnimatedGeoNode::getSkinningMatrices()
    {
      std::vector<util::Matrix<float, 4>> skinningMatrices;
      skinningMatrices.resize(m_inverseBindPoseMatrices.size());

      for(unsigned int i = 0; i < skinningMatrices.size(); i++)
      {
        skinningMatrices[i] = m_boneTransformMatrices[i] * m_inverseBindPoseMatrices[i];
      }

      return skinningMatrices;
    }
	}
}