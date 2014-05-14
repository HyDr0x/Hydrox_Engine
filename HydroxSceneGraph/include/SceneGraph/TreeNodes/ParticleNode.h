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
      virtual ~ParticleNode();

      ParticleNode& operator=(const ParticleNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      virtual bool isParticleNode() const;

      void setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix);
      util::Matrix<float, 4> getTransformationMatrix() const;

    private:

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix
    };
	}
}

#endif
