#ifndef LODNODE_H_
#define LODNODE_H_

#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"
#include "Hydrox/Utility/Tree/GroupNode.h"

namespace he
{
  class Traverser;

  class GRAPHICAPI LODNode : public GroupNode
  {
  public:

    LODNode(Vector<float, 3> position, unsigned int lodLevel, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr, TreeNode* firstChild = nullptr);
    LODNode& operator=(const LODNode& sourceNode);
    virtual TreeNode& operator=(const TreeNode& sourceNode);
    virtual ~LODNode();

    virtual GroupNode* clone() const;

    virtual bool ascendTraverse(Traverser* traverser);
    virtual bool preTraverse(Traverser* traverser);
    virtual void postTraverse(Traverser* traverser);

    unsigned int getLODLevel();
    bool getLOD(Vector<float, 3> camPos, const std::vector<float>& lodRanges);

    void transformPosition(Vector<float, 3>& translation, float& scale, Quaternion<float>& rotation);

  private:

    Vector<float, 3> m_position;
    Vector<float, 3> m_transformedPosition;

    unsigned int m_lodLevel;
  };
}

#endif
