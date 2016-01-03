#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"

#include <cassert>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
  namespace sg
  {
    AnimatedGeoNode::AnimatedGeoNode(const std::vector<util::Matrix<float, 4>>& inverseBindPoseMatrices, util::SharedPointer<util::EventManager> eventManager, util::ResourceHandle meshHandle, util::ResourceHandle materialHandle,
      const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling)
      : GeoNode(eventManager, meshHandle, materialHandle, nodeName, parent, nextSibling), m_inverseBindPoseMatrices(inverseBindPoseMatrices)
    {
      m_boneTransformMatrices.resize(m_inverseBindPoseMatrices.size());
      m_index.nodeType = ANIMATEDGEONODE;
    }

    AnimatedGeoNode::AnimatedGeoNode(const AnimatedGeoNode& sourceNode) : GeoNode(sourceNode)
    {
      m_inverseBindPoseMatrices = sourceNode.m_inverseBindPoseMatrices;
      m_boneTransformMatrices = sourceNode.m_boneTransformMatrices;
    }

    AnimatedGeoNode::AnimatedGeoNode(const TreeNode& sourceNode) : GeoNode(sourceNode)
    {
      assert(ANIMATEDGEONODE == sourceNode.getNodeType());

      const AnimatedGeoNode& copyNode = static_cast<const AnimatedGeoNode&>(sourceNode);

      this->~AnimatedGeoNode();
      new (this) AnimatedGeoNode(copyNode);
    }

    AnimatedGeoNode::~AnimatedGeoNode()
    {
    }

    TreeNode& AnimatedGeoNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~AnimatedGeoNode();

      new (this) AnimatedGeoNode(sourceNode);

      return *this;
    }

    TreeNode* AnimatedGeoNode::clone() const
    {
      AnimatedGeoNode *newNode = new AnimatedGeoNode(m_inverseBindPoseMatrices, m_eventManager, m_meshHandle, m_materialHandle, m_nodeName);

      newNode->m_trfMatrix = m_trfMatrix;

      newNode->m_boneTransformMatrices = m_boneTransformMatrices;

      return newNode;
    }

    bool AnimatedGeoNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    bool AnimatedGeoNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void AnimatedGeoNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool AnimatedGeoNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool AnimatedGeoNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void AnimatedGeoNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
    }

    void AnimatedGeoNode::setMeshHandle(util::ResourceHandle meshHandle)
    {
      if(m_renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnRemoveGeometryNode)->execute(util::SharedPointer<const xBar::IGeometryContainer>(new xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::SKINNEDNODE), &m_trfMatrix, m_materialHandle, m_meshHandle)));
      }

      m_meshHandle = meshHandle;

      if(m_renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnAddGeometryNode)->execute(util::SharedPointer<const xBar::IGeometryContainer>(new xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::SKINNEDNODE), &m_trfMatrix, m_materialHandle, m_meshHandle)));
      }
    }

    void AnimatedGeoNode::setMaterialHandle(util::ResourceHandle materialHandle)
    {
      if(m_renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnRemoveGeometryNode)->execute(util::SharedPointer<const xBar::IGeometryContainer>(new xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::SKINNEDNODE), &m_trfMatrix, m_materialHandle, m_meshHandle)));
      }

      m_materialHandle = materialHandle;

      if(m_renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnAddGeometryNode)->execute(util::SharedPointer<const xBar::IGeometryContainer>(new xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::SKINNEDNODE), &m_trfMatrix, m_materialHandle, m_meshHandle)));
      }
    }

    void AnimatedGeoNode::setRenderable(bool renderable)
    {
      if(!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnAddGeometryNode)->execute(util::SharedPointer<const xBar::IGeometryContainer>(new xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::SKINNEDNODE), &m_trfMatrix, m_materialHandle, m_meshHandle)));
      }
      else if(m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnRemoveGeometryNode)->execute(util::SharedPointer<const xBar::IGeometryContainer>(new xBar::SkinnedGeometryContainer(&m_boneTransformMatrices, &m_inverseBindPoseMatrices, util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::SKINNEDNODE), &m_trfMatrix, m_materialHandle, m_meshHandle)));
      }
      m_renderable = renderable;
    }

    void AnimatedGeoNode::setBoneTransform(const util::Matrix<float, 4>& boneTransform, unsigned int boneIndex)
    {
      m_boneTransformMatrices[boneIndex] = boneTransform;
    }

    std::vector<util::Matrix<float, 4>> AnimatedGeoNode::getSkinningMatrices() const
    {
      std::vector<util::Matrix<float, 4>> skinningMatrices;
      skinningMatrices.resize(m_inverseBindPoseMatrices.size());

      for(unsigned int i = 0; i < skinningMatrices.size(); i++)
      {
        skinningMatrices[i] = m_boneTransformMatrices[i] * m_inverseBindPoseMatrices[i];
      }

      return skinningMatrices;
    }

    std::vector<util::Matrix<float, 4>> AnimatedGeoNode::getInverseBindPoseMatrices() const
    {
      return m_inverseBindPoseMatrices;
    }

    void AnimatedGeoNode::setInverseBindPoseMatrices(const std::vector<util::Matrix<float, 4>>& inverseBindPoseMatrices)
    {
      m_inverseBindPoseMatrices = inverseBindPoseMatrices;
    }

    void AnimatedGeoNode::read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      GeoNode::read(stream, eventManager, resourceHandles);

      unsigned int inverseBindPoseMatricesSize;
      stream >> inverseBindPoseMatricesSize;

      m_inverseBindPoseMatrices.resize(inverseBindPoseMatricesSize);
      if(m_inverseBindPoseMatrices.size())
      {
        std::getline(stream, std::string());
        stream.read((char*)&m_inverseBindPoseMatrices[0][0][0], sizeof(m_inverseBindPoseMatrices[0]) * m_inverseBindPoseMatrices.size());
      }

      m_boneTransformMatrices.resize(m_inverseBindPoseMatrices.size());
    }

    void AnimatedGeoNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::ResourceHandle::Less>>& resourceHandles) const
    {
      GeoNode::write(stream, resourceHandles);

      stream << m_inverseBindPoseMatrices.size() << std::endl;
      if(m_inverseBindPoseMatrices.size())
      {
        stream.write((char*)&m_inverseBindPoseMatrices[0][0][0], sizeof(m_inverseBindPoseMatrices[0]) * m_inverseBindPoseMatrices.size());
        stream << std::endl;
      }
    }
  }
}