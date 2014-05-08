#include "SceneGraph/TreeNodes/GeoNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

#include <XBar/StaticGeometryContainer.h>

namespace he
{
	namespace sg
	{
    GeoNode::GeoNode(util::EventManager *eventManager, util::ResourceHandle meshHandle, util::ResourceHandle materialHandle, bool transparency, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                                                                                                                          m_eventManager(eventManager),
                                                                                                                                                                                                                          m_meshHandle(meshHandle),
                                                                                                                                                                                                                          m_materialHandle(materialHandle),
                                                                                                                                                                                                                          m_transparency(transparency),
                                                                                                                                                                                                                          m_renderable(false)
    {
      m_trfMatrix = util::Matrix<float, 4>::identity();
    }

    GeoNode& GeoNode::operator=(const GeoNode& sourceNode)
    {
      TreeNode::operator=(sourceNode);

      m_eventManager = sourceNode.m_eventManager;
      m_materialHandle = sourceNode.m_materialHandle;
      m_trfMatrix = sourceNode.m_trfMatrix;
      m_meshHandle = sourceNode.m_meshHandle;
      m_renderable = sourceNode.m_renderable;
      m_transparency = sourceNode.m_transparency;

      return *this;
    }

    TreeNode& GeoNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const GeoNode& copyNode = static_cast<const GeoNode&>(sourceNode);
      GeoNode::operator=(copyNode);

      return *this;
    }

    GeoNode::~GeoNode()
    {
    }

    TreeNode* GeoNode::clone() const
    {
      GeoNode *newNode = new GeoNode(m_eventManager, m_meshHandle, m_materialHandle, m_transparency, m_nodeName);

      newNode->m_trfMatrix = m_trfMatrix;

      return newNode;
    }

    bool GeoNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(this);
    }

    bool GeoNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void GeoNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool GeoNode::isGeoNode()
    {
      return true;
    }

    void GeoNode::setMeshHandle(util::ResourceHandle meshHandle)
    {
      if(m_renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::StaticGeometryContainer& staticGeometry)>(util::EventManager::OnRemoveGeometryNode)->execute(xBar::StaticGeometryContainer(&m_trfMatrix, m_materialHandle, m_meshHandle));
      }

      m_meshHandle = meshHandle;

      if(m_renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::StaticGeometryContainer& staticGeometry)>(util::EventManager::OnAddGeometryNode)->execute(xBar::StaticGeometryContainer(&m_trfMatrix, m_materialHandle, m_meshHandle));
      }
    }

    util::ResourceHandle GeoNode::getMeshHandle() const
    {
      return m_meshHandle;
    }

    void GeoNode::setMaterialHandle(util::ResourceHandle materialHandle)
    {
      if(m_renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::StaticGeometryContainer& staticGeometry)>(util::EventManager::OnRemoveGeometryNode)->execute(xBar::StaticGeometryContainer(&m_trfMatrix, m_materialHandle, m_meshHandle));
      }

      m_materialHandle = materialHandle;

      if(m_renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::StaticGeometryContainer& staticGeometry)>(util::EventManager::OnAddGeometryNode)->execute(xBar::StaticGeometryContainer(&m_trfMatrix, m_materialHandle, m_meshHandle));
      }
    }

    util::ResourceHandle GeoNode::getMaterialHandle() const
    {
      return m_materialHandle;
    }

    util::Matrix<float,4> GeoNode::getTransformationMatrix() const
    {
      return m_trfMatrix;
    }

    void GeoNode::setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix)
    {
      m_trfMatrix = trfMatrix;
    }

    void GeoNode::setRenderable(bool renderable)
    {
      if(!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::StaticGeometryContainer& staticGeometry)>(util::EventManager::OnAddGeometryNode)->execute(xBar::StaticGeometryContainer(&m_trfMatrix, m_materialHandle, m_meshHandle));
      }
      else if(m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void (*)(xBar::StaticGeometryContainer& staticGeometry)>(util::EventManager::OnRemoveGeometryNode)->execute(xBar::StaticGeometryContainer(&m_trfMatrix, m_materialHandle, m_meshHandle));
      }

      m_renderable = renderable;
    }

    bool GeoNode::getRenderable() const
    {
      return m_renderable;
    }

    bool GeoNode::getTransparency() const
    {
      return m_transparency;
    }
	}
}