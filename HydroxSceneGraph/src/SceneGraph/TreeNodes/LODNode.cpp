#include "SceneGraph/TreeNodes/LODNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
	namespace sg
	{
    LODNode::LODNode(util::Vector<float, 3> position, unsigned int lodLevel, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : 
    GroupNode(nodeName, parent, nextSibling, firstChild),
      m_lodLevel(lodLevel),
      m_position(position)
    {
      m_dirtyFlag |= LOD_INRANGE;
      m_transformedPosition = m_position;
    }

    LODNode& LODNode::operator=(const LODNode& sourceNode)
    {
      GroupNode::operator=(sourceNode);

      m_lodLevel = sourceNode.m_lodLevel;
      m_position = sourceNode.m_position;
      m_transformedPosition = sourceNode.m_transformedPosition;

      return *this;
    }

    TreeNode& LODNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const LODNode& copyNode = static_cast<const LODNode&>(sourceNode);
      LODNode::operator=(copyNode);

      return *this;
    }

    LODNode::~LODNode()
    {
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
      return traverser->ascendTraverse(this);
    }

    bool LODNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void LODNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool LODNode::isLODNode()
    {
      return true;
    }

    unsigned int LODNode::getLODLevel()
    {
      return m_lodLevel;
    }

    bool LODNode::getLOD(util::Vector<float, 3> camPos, const std::vector<float>& lodRanges)
    {
      float distance = static_cast<float>((m_transformedPosition - camPos).length());

	    return lodRanges[m_lodLevel] < distance && distance < lodRanges[std::max(m_lodLevel + 1, m_lodLevel)];
    }

    void LODNode::transformPosition(util::Vector<float, 3>& translation, float& scale, util::Quaternion<float>& rotation)
    {
      m_transformedPosition = translation + rotation.apply(m_position * scale);
    }
	}
}