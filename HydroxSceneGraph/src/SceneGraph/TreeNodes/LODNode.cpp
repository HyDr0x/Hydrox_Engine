#include "SceneGraph/TreeNodes/LODNode.h"

#include <assert.h>

#include <algorithm>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    LODNode::LODNode(util::Vector<float, 3> position, unsigned int lodLevel, const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling, NodeIndex firstChild) :
    GroupNode(nodeName, parent, nextSibling, firstChild),
      m_lodLevel(lodLevel),
      m_position(position)
    {
      m_dirtyFlag |= LOD_INRANGE;
      m_transformedPosition = m_position;

      m_index.nodeType = LODNODE;
    }

    LODNode::LODNode(const LODNode& sourceNode) : GroupNode(sourceNode)
    {
      m_lodLevel = sourceNode.m_lodLevel;
      m_position = sourceNode.m_position;
      m_dirtyFlag |= LOD_INRANGE;
      m_transformedPosition = m_position;
    }

    LODNode::LODNode(const TreeNode& sourceNode) : GroupNode(sourceNode)
    {
      assert(LODNODE == sourceNode.getNodeType());

      const LODNode& copyNode = static_cast<const LODNode&>(sourceNode);

      new (this) LODNode(copyNode);
    }

    LODNode::~LODNode()
    {
    }

    TreeNode& LODNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~LODNode();

      new (this) LODNode(sourceNode);

      return *this;
    }

    GroupNode* LODNode::clone() const
    {
      LODNode *newNode = new LODNode(m_position, m_lodLevel, m_nodeName);

      newNode->m_nodeName = m_nodeName;
      newNode->m_dirtyFlag = m_dirtyFlag;
      newNode->m_position = m_position;
      newNode->m_transformedPosition = m_transformedPosition;

      return newNode;
    }

    bool LODNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    bool LODNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void LODNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool LODNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool LODNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void LODNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
    }

    void LODNode::setLODLevel(unsigned int lodLevel)
    {
      m_lodLevel = lodLevel;
    }

    unsigned int LODNode::getLODLevel() const
    {
      return m_lodLevel;
    }

    bool LODNode::getLOD(const util::Vector<float, 3>& camPos, const std::vector<float>& lodRanges) const
    {
      float distance = static_cast<float>((m_transformedPosition - camPos).length());

      return lodRanges[m_lodLevel] < distance && distance < lodRanges[std::max(m_lodLevel + 1, m_lodLevel)];
    }

    void LODNode::transformPosition(const util::Vector<float, 3>& translation, float scale, const util::Quaternion<float>& rotation)
    {
      m_transformedPosition = translation + rotation.apply(m_position * scale);
    }

    void LODNode::setPosition(util::Vector<float, 3> position)
    {
      m_position = position;
    }

    util::Vector<float, 3> LODNode::getPosition() const
    {
      return m_position;
    }

    void LODNode::read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles)
    {
      GroupNode::read(stream, eventManager, resourceHandles);

      stream >> m_position;
      stream >> m_lodLevel;

      m_dirtyFlag |= LOD_INRANGE;
      m_transformedPosition = m_position;
    }

    void LODNode::write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const
    {
      GroupNode::write(stream, resourceHandles);

      stream << m_position << std::endl;
      stream << m_lodLevel << std::endl;
    }
  }
}