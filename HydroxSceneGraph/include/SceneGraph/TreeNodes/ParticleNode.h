#ifndef PARTICLENODE_H_
#define PARTICLENODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
  namespace util
  {
    class EventManager;
  }

  namespace sg
  {
    class Traverser;

    class GRAPHICAPI ParticleNode : public TreeNode
    {
    public:

      ParticleNode(util::EventManager *eventManager, const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0);
      ParticleNode(const ParticleNode& sourceNode);
      ParticleNode(const TreeNode& sourceNode);
      virtual ~ParticleNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      void setRenderable(bool renderable);
      bool getRenderable() const;

      void setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix);
      util::Matrix<float, 4> getTransformationMatrix() const;

    private:

      util::EventManager *m_eventManager;

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix

      bool m_renderable;
    };
  }
}

#endif
