#ifndef ANIMATIONCONTROLTRAVERSER_H_
#define ANIMATIONCONTROLTRAVERSER_H_

#include <string>

#include "Hydrox/Utility/Traverser/Traverser.h"

namespace he
{
  class AnimationControlTraverser : public Traverser
  {
  public:

    AnimationControlTraverser();
    virtual ~AnimationControlTraverser();

    void setAnimationTime(float animationTime);
    void addAnimationTime(float animationTime);
    void setPauseAnimation(bool pauseAnimation);//pause/unpause animation
    void setStopAnimation();

    virtual bool preTraverse(AnimatedTransformNode* treeNode);
    virtual void postTraverse(AnimatedTransformNode* treeNode);

    virtual bool preTraverse(TreeNode* treeNode);
    virtual void postTraverse(TreeNode* treeNode);

  protected:

    float m_setterAnimationTime;
    float m_adderAnimationTime;
    bool m_setAnimationTime;
    bool m_addAnimationTime;
    bool m_pauseAnimation;
    bool m_stopAnimation;
  };
}

#endif
