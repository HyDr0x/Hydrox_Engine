#ifndef PARTICLENODE_H_
#define PARTICLENODE_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Utility/Tree/TreeNode.h"

namespace he
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

    Matrix<float, 4> getTransformationMatrix() const;
    void setTransformationMatrix(const Matrix<float, 4>& trfMatrix);

  private:

    Matrix<float, 4> m_trfMatrix;//the tranformation Matrix
  };
}

#endif
