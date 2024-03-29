#ifndef LIGHTNODE_H_
#define LIGHTNODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
	namespace sg
	{
    class Traverser;

    class GRAPHICAPI LightNode : public TreeNode
    {
    public:

      LightNode(const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      LightNode& operator=(const LightNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);
      virtual ~LightNode();

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool isLightNode();

      util::Matrix<float, 4> getTransformationMatrix() const;
      void setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix);

    private:

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix
    };
	}
}

#endif
