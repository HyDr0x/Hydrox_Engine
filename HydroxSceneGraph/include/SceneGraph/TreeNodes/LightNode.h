#ifndef LIGHTNODE_H_
#define LIGHTNODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

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

    class GRAPHICAPI LightNode : public TreeNode
    {
    public:

      LightNode(util::EventManager *eventManager, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      virtual ~LightNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      virtual bool isLightNode() const;

      void setRenderable(bool renderable);

      void setLightHandle(util::ResourceHandle lightHandle);
      util::ResourceHandle getLightHandle() const;

      void setTransformationMatrix(const util::Matrix<float, 4>& trfMatrix);
      util::Matrix<float, 4> getTransformationMatrix() const;

    private:

      util::EventManager *m_eventManager;

      util::Matrix<float, 4> m_trfMatrix;//the tranformation util::Matrix

      util::ResourceHandle m_lightHandle;

      bool m_renderable;
    };
  }
}

#endif
