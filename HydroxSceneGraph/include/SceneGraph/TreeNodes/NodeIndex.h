#ifndef NODEINDEX_H_
#define NODEINDEX_H_

#include <string>

#include "SceneGraph/DLLExport.h"

namespace he
{
  namespace sg
  {    enum NodeType
    {
      TREENODE,
      GROUPNODE,
      GEONODE,
      ANIMATEDGEONODE,
      TRANSFORMNODE,
      ANIMATEDTRANSFORMNODE,
      BILLBOARDNODE,
      LODNODE,
      LIGHTNODE,
      SHADOWLIGHTNODE,
      PARTICLENODE,
      NODENUMBER,
    };    class ConstTraverser;
    class Traverser;
    class GroupNode;

    struct NodeIndex
    {
      class Less
      {
      public:
        inline bool operator()(const NodeIndex& o1, const NodeIndex& o2) const
        {
          return o1.index < o2.index;
        }
      };

      NodeIndex(unsigned int newIndex = ~0, NodeType newNodeType = TREENODE) : index(newIndex), nodeType(newNodeType)
      {
      }

      bool operator==(const NodeIndex& other)
      {
        return index == other.index && nodeType == other.nodeType;
      }

      bool operator!=(const NodeIndex& other)
      {
        return index != other.index || nodeType != other.nodeType;
      }

      unsigned int index;
      NodeType nodeType;
    };
  }
}

#endif
