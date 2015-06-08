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
      LODNode(util::vec3f position, unsigned int lodLevel, const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0, NodeIndex firstChild = ~0);
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
      bool getLOD(const util::vec3f& camPos, const std::vector<float>& lodRanges) const;

      void transformPosition(const util::vec3f& translation, float scale, const util::Quaternion<float>& rotation);

      void setPosition(util::vec3f position);
      util::vec3f getPosition() const;

      virtual void read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::ResourceHandle::Less>>& resourceHandles) const;

    private:

      util::vec3f m_position;
      util::vec3f m_transformedPosition;

      unsigned int m_lodLevel;
    };
  }
}

#endif
