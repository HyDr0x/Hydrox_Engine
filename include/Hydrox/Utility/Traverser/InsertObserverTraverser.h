#ifndef INSERTOBSERVERTRAVERSER_H_
#define INSERTOBSERVERTRAVERSER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Traverser/Traverser.h"

class Scene;

class GRAPHICAPI InsertObserverTraverser : public Traverser
{
public:

  InsertObserverTraverser(Scene *scene);
  virtual ~InsertObserverTraverser();

  virtual bool preTraverse(TransformNode* treeNode);
  virtual void postTraverse(TransformNode* treeNode);

protected:

  Scene *m_scene;
};

#endif
