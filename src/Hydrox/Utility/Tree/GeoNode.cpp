#include "Hydrox/Utility/Tree/GeoNode.h"

#include <assert.h>

#include "Hydrox/Utility/Traverser/Traverser.h"

namespace he
{
  GeoNode::GeoNode(ResourceHandle meshIndex, bool renderable, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling) : TreeNode(nodeName, parent, nextSibling),
                                                                                                                                    m_meshIndex(meshIndex),
                                                                                                                                    m_renderable(renderable)
  {
    m_trfMatrix = Matrix<float, 4>::identity();
  }

  GeoNode& GeoNode::operator=(const GeoNode& sourceNode)
  {
    TreeNode::operator=(sourceNode);

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
    GeoNode *newNode = new GeoNode(m_meshIndex, true, m_nodeName);

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

  ResourceHandle GeoNode::getMeshIndex() const
  {
    return m_meshIndex;
  }

  Matrix<float,4> GeoNode::getTransformationMatrix() const
  {
    return m_trfMatrix;
  }

  void GeoNode::setTransformationMatrix(const Matrix<float,4>& trfMatrix)
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