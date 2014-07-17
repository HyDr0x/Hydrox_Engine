#ifndef GETGLOBALCOORDINATETRAVERSER_H_
#define GETGLOBALCOORDINATETRAVERSER_H_

#include <stack>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace sg
  {    class GetGlobalCoordinateTraverser : public ConstTraverser
    {
    public:

      GetGlobalCoordinateTraverser(const TreeNodeAllocator& allocator);
      virtual ~GetGlobalCoordinateTraverser();

      void clearStacks();

      virtual bool ascendTraverse(const AnimatedTransformNode& treeNode);
      virtual bool ascendTraverse(const TransformNode& treeNode);

      float getGlobalScale() const;
      util::Vector<float, 3> getGlobalTranslation() const;
      util::Quaternion<float> getGlobalRotation() const;

    protected:

      virtual void postAscendTraverse();

      std::stack<float> m_scaleStack;
      std::stack<util::Vector<float, 3>> m_translateStack;
      std::stack<util::Quaternion<float>> m_rotationStack;

      float m_globalScale;
      util::Vector<float, 3> m_globalTranslation;
      util::Quaternion<float> m_globalRotation;
    };
  }
}

#endif
