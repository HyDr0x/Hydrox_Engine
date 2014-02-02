#ifndef PARTICLENODE_H_
#define PARTICLENODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
	namespace sg
	{
    class Traverser;

    class GRAPHICAPI ParticleNode : public TreeNode
    {
    public:

      ParticleNode(const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      ParticleNode& operator=(const ParticleNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);
      virtual ~ParticleNode();

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual ParticleNode* getNodeType();

      util::Matrix<float, 4> getTransformationMatrix() const;
      void setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix);

    private:

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix
    };
	}
}

#endif
