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

      LODNode(util::Vector<float, 3> position, unsigned int lodLevel, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
      LODNode& operator=(const LODNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);
      virtual ~LODNode();

      virtual GroupNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool isLODNode();

      unsigned int getLODLevel();
      bool getLOD(util::Vector<float, 3> camPos, const std::vector<float>& lodRanges);

      void transformPosition(util::Vector<float, 3>& translation, float& scale, util::Quaternion<float>& rotation);

    private:

      util::Vector<float, 3> m_position;
      util::Vector<float, 3> m_transformedPosition;

      unsigned int m_lodLevel;
    };
	}
}

#endif
