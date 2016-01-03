#include "SceneGraph/TreeNodes/GeoNode.h"

#include <cassert>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

#include <XBar/StaticGeometryContainer.h>

namespace he
{
  namespace sg
  {
    GeoNode::GeoNode(util::SharedPointer<util::EventManager> eventManager, util::ResourceHandle meshHandle, util::ResourceHandle materialHandle, const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling) :
      TreeNode(nodeName, parent, nextSibling),
      m_eventManager(eventManager),
      m_meshHandle(meshHandle),
      m_materialHandle(materialHandle),
      m_renderable(false)
    {
      m_trfMatrix = util::Matrix<float, 4>::identity();
      m_index.nodeType = GEONODE;
    }

    GeoNode::GeoNode(const GeoNode& sourceNode) : TreeNode(sourceNode)
    {
      m_eventManager = sourceNode.m_eventManager;
      m_meshHandle = sourceNode.m_meshHandle;
      m_materialHandle = sourceNode.m_materialHandle;
      m_renderable = sourceNode.m_renderable;
      m_trfMatrix = util::Matrix<float, 4>::identity();
    }

    GeoNode::GeoNode(const TreeNode& sourceNode) : TreeNode(sourceNode)
    {
      assert(GEONODE == sourceNode.getNodeType() || ANIMATEDGEONODE == sourceNode.getNodeType());

      const GeoNode& copyNode = static_cast<const GeoNode&>(sourceNode);

      this->~GeoNode();
      new (this) GeoNode(copyNode);
    }

    GeoNode::~GeoNode()
    {
    }

    TreeNode& GeoNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~GeoNode();

      new (this) GeoNode(sourceNode);

      return *this;
    }

    TreeNode* GeoNode::clone() const
    {
      GeoNode *newNode = new GeoNode(m_eventManager, m_meshHandle, m_materialHandle, m_nodeName);

      newNode->m_trfMatrix = m_trfMatrix;

      return newNode;
    }

    bool GeoNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    bool GeoNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void GeoNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool GeoNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool GeoNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void GeoNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
    }

    void GeoNode::setEventManager(util::SharedPointer<util::EventManager> eventManager)
    {
      m_eventManager = eventManager;
    }

    void GeoNode::setMeshHandle(util::ResourceHandle meshHandle)
    {
      if(m_renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnRemoveGeometryNode)->execute(
          util::SharedPointer<const xBar::IGeometryContainer>(new xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE), &m_trfMatrix, m_materialHandle, m_meshHandle))
          );
      }

      m_meshHandle = meshHandle;

      if(m_renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnAddGeometryNode)->execute(
          util::SharedPointer<const xBar::IGeometryContainer>(new xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE), &m_trfMatrix, m_materialHandle, m_meshHandle))
          );
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
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnRemoveGeometryNode)->execute(
          util::SharedPointer<const xBar::IGeometryContainer>(new xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE), &m_trfMatrix, m_materialHandle, m_meshHandle))
          );
      }

      m_materialHandle = materialHandle;

      if(m_renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnAddGeometryNode)->execute(
          util::SharedPointer<const xBar::IGeometryContainer>(new xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE), &m_trfMatrix, m_materialHandle, m_meshHandle))
          );
      }
    }

    util::ResourceHandle GeoNode::getMaterialHandle() const
    {
      return m_materialHandle;
    }

    void GeoNode::setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix)
    {
      m_trfMatrix = trfMatrix;
    }

    util::Matrix<float,4> GeoNode::getTransformationMatrix() const
    {
      return m_trfMatrix;
    }

    void GeoNode::setRenderable(bool renderable)
    {
      if(!m_renderable && renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnAddGeometryNode)->execute(
          util::SharedPointer<const xBar::IGeometryContainer>(new xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE), &m_trfMatrix, m_materialHandle, m_meshHandle))
          );
      }
      else if(m_renderable && !renderable)
      {
        m_eventManager->raiseSignal<void(*)(util::SharedPointer<const xBar::IGeometryContainer> geometry)>(util::EventManager::OnRemoveGeometryNode)->execute(
          util::SharedPointer<const xBar::IGeometryContainer>(new xBar::StaticGeometryContainer(util::Flags<xBar::RenderNodeType>::convertToFlag(xBar::STATICNODE), &m_trfMatrix, m_materialHandle, m_meshHandle))
          );
      }

      m_renderable = renderable;
    }

    bool GeoNode::getRenderable() const
    {
      return m_renderable;
    }

    void GeoNode::read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      TreeNode::read(stream, eventManager, resourceHandles);
      std::string resourceFilename;

      std::getline(stream, resourceFilename);
      std::getline(stream, resourceFilename);//getline because of white spaces
      m_meshHandle = resourceHandles["Meshes"][resourceFilename];

      std::getline(stream, resourceFilename);//getline because of white spaces
      m_materialHandle = resourceHandles["Materials"][resourceFilename];

      m_eventManager = eventManager;

      m_renderable = false;
      m_trfMatrix = util::Matrix<float, 4>::identity();
    }

    void GeoNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::ResourceHandle::Less>>& resourceHandles) const
    {
      TreeNode::write(stream, resourceHandles);

      stream << resourceHandles.find("Meshes")->second.find(m_meshHandle)->second << std::endl;
      stream << resourceHandles.find("Materials")->second.find(m_materialHandle)->second << std::endl;
    }
  }
}