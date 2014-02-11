#ifndef BILLBOARDNODE_H_
#define BILLBOARDNODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
	namespace sg
	{
    class Traverser;

    class GRAPHICAPI BillboardNode : public TreeNode
    {
    public:

      BillboardNode(util::ResourceHandle billboardIndex, bool renderable, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      BillboardNode& operator=(const BillboardNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);
      virtual ~BillboardNode();

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool isBillboardNode();

      util::ResourceHandle getBillboardIndex() const;

      util::Matrix<float, 4> getTransformationMatrix() const;
      void setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix);

      void setRenderable(bool renderable);
	    bool getRenderable() const;

    private:

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix

      util::ResourceHandle m_billboardIndex;//index which points into the array of the texturemanager

      bool m_renderable;//boolean which decides if the mesh is being drawn or not
    };
	}
}

#endif
