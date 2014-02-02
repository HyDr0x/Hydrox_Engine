#include "SceneGraph/TreeNodes/GeoNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
	namespace sg
	{
    GeoNode::GeoNode(util::ResourceHandle meshIndex, util::ResourceHandle materialIndex, bool renderable, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                                                                       m_meshIndex(meshIndex),
                                                                                                                                                                       m_materialIndex(materialIndex),
                                                                                                                                                                       m_renderable(renderable)
    {
      m_trfMatrix = util::Matrix<float, 4>::identity();
    }

    GeoNode& GeoNode::operator=(const GeoNode& sourceNode)
    {
      TreeNode::operator=(sourceNode);

      m_materialIndex = sourceNode.m_materialIndex;
      m_trfMatrix = sourceNode.m_trfMatrix;
      m_meshIndex = sourceNode.m_meshIndex;
      m_renderable = sourceNode.m_renderable;

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
      GeoNode *newNode = new GeoNode(m_meshIndex, m_materialIndex, true, m_nodeName);

      newNode->m_nodeName = m_nodeName;

      newNode->m_trfMatrix = m_trfMatrix;
      newNode->m_renderable = m_renderable;

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

    GeoNode* GeoNode::getNodeType()
    {
      return this;
    }

    void GeoNode::setMeshIndex(util::ResourceHandle meshIndex)
    {
      m_meshIndex = meshIndex;
    }

    util::ResourceHandle GeoNode::getMeshIndex() const
    {
      return m_meshIndex;
    }

    void GeoNode::setMaterial(util::ResourceHandle materialIndex)
    {
      m_materialIndex = materialIndex;
    }

    util::ResourceHandle GeoNode::getMaterial() const
    {
      return m_materialIndex;
    }

    util::Matrix<float,4> GeoNode::getTransformationMatrix() const
    {
      return m_trfMatrix;
    }

    void GeoNode::setTransformationMatrix(const util::Matrix<float,4>& trfMatrix)
    {
      m_trfMatrix = trfMatrix;
    }

    void GeoNode::setRenderable(bool renderable)
    {
      m_renderable = renderable;
    }

    bool GeoNode::getRenderable() const
    {
      return m_renderable;
    }
	}
}