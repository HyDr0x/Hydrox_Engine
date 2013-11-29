#ifndef DELETETRAVERSER_H_
#define DELETETRAVERSER_H_

#include "Hydrox/Utility/Traverser/Traverser.h"

namespace he
{
  class DeleteTraverser : public Traverser
  {
  public:

    DeleteTraverser();
    virtual ~DeleteTraverser();

    virtual bool preTraverse(TreeNode* treeNode);
    virtual void postTraverse(TreeNode* treeNode);

  protected:

  };
}

#endif
