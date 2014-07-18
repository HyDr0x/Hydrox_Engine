#include "SceneGraph/Traverser/AnimationControlTraverser.h"

#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
  namespace sg
  {
    AnimationControlTraverser::AnimationControlTraverser(TreeNodeAllocator& allocator) : Traverser(allocator)
    {
      m_setterAnimationTime = 0.0f;
      m_adderAnimationTime = 0.0f;
      m_setAnimationTime = false;
      m_addAnimationTime = false;
      m_pauseAnimation = false;
      m_stopAnimation = false;
    }

    AnimationControlTraverser::~AnimationControlTraverser()
    {
    }

    void AnimationControlTraverser::setAnimationTime(float animationTime)
    {
      m_setAnimationTime = true;
      m_setterAnimationTime = animationTime;
    }

    void AnimationControlTraverser::addAnimationTime(float animationTime)
    {
      m_addAnimationTime = true;
      m_adderAnimationTime = animationTime;
    }

    void AnimationControlTraverser::setPauseAnimation(bool pauseAnimation)
    {
      m_pauseAnimation = pauseAnimation;
    }

    void AnimationControlTraverser::setStopAnimation()
    {
      m_stopAnimation = true;
    }

    bool AnimationControlTraverser::preTraverse(AnimatedTransformNode& treeNode)
    {
      treeNode.setPauseAnimation(m_pauseAnimation);//pause/unpause animation

      if(m_stopAnimation)
      {
        treeNode.stopAnimation();
      }

      if(m_setAnimationTime)
      {
        treeNode.setCurrentAnimationTime(m_setterAnimationTime);
      }
    
      if(m_addAnimationTime)
      {
        treeNode.addCurrentAnimationTime(m_adderAnimationTime);
      }

      return true;
    }

    void AnimationControlTraverser::postTraverse(AnimatedTransformNode& treeNode)
    {
    }

    bool AnimationControlTraverser::preTraverse(TreeNode& treeNode)
    {
      return false;
    }

    void AnimationControlTraverser::postTraverse(TreeNode& treeNode)
    {
    }
  }
}