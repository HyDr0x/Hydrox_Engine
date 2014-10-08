#ifndef LODNODE_H_
#define LODNODE_H_

#include <vector>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include "SceneGraph/TreeNodes/GroupNode.h"

namespace he
{
  namespace sg
  {
    class Traverser;

    class GRAPHICAPI LODNode : public GroupNode
    {
    public:

      LODNode(){}
      LODNode(util::Vector<float, 3> position, unsigned int lodLevel, const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0, NodeIndex firstChild = ~0);
      LODNode(const LODNode& sourceNode);
      LODNode(const TreeNode& sourceNode);
      virtual ~LODNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual GroupNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      void setLODLevel(unsigned int lodLevel);
      unsigned int getLODLevel() const;
      bool getLOD(const util::Vector<float, 3>& camPos, const std::vector<float>& lodRanges) const;

      void transformPosition(const util::Vector<float, 3>& translation, float scale, const util::Quaternion<float>& rotation);

      void setPosition(util::Vector<float, 3> position);
      util::Vector<float, 3> getPosition() const;

      virtual void read(std::istream& stream, util::EventManager *eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const;

    private:

      util::Vector<float, 3> m_position;
      util::Vector<float, 3> m_transformedPosition;

      unsigned int m_lodLevel;
    };
  }
}

#endif
