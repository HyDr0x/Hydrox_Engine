#include "Hydrox/Utility/Traverser/TransformTraverser.h"

#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

TransformTraverser::TransformTraverser()
{
}

TransformTraverser::~TransformTraverser()
{
}

void TransformTraverser::insertTransformMatrix(Mat<float, 4>& trfMatrix)
{
  m_matrixStack.push(trfMatrix);
}

std::stack<Mat<float, 4>>& TransformTraverser::getMatrixStack()
{
  return m_matrixStack;
}

bool TransformTraverser::ascendTraverse(TransformNode* treeNode)
{
  if(treeNode->getDirtyFlag() & GroupNode::TRF_DIRTY)
  {
    while(!m_matrixStack.empty())//clear the matrix stack
    {
      m_matrixStack.pop();
    }

    return false;
  }

  Mat<float, 4> trfMatrix = Mat<float, 4>::identity();
  treeNode->calculateTransformation(trfMatrix);
  m_matrixStack.push(trfMatrix);

  return true;
}

bool TransformTraverser::preTraverse(TransformNode* treeNode)
{
  treeNode->removeDirtyFlag(GroupNode::TRF_DIRTY);//transformation is updated now

  Mat<float, 4> trfMatrix;
  if(!m_matrixStack.empty())
  {
    trfMatrix = m_matrixStack.top();
  }
  else
  {
    trfMatrix = Mat<float, 4>::identity();
  }

  treeNode->calculateTransformation(trfMatrix);
  m_matrixStack.push(trfMatrix);

  return true;
}

void TransformTraverser::postTraverse(TransformNode* treeNode)
{
  m_matrixStack.pop();
}

bool TransformTraverser::ascendTraverse(LODNode* treeNode)
{
  if(treeNode->getDirtyFlag() & GroupNode::TRF_DIRTY)
  {
    while(!m_matrixStack.empty())//clear the matrix stack, cause there is another dirty node higher in the tree
    {
      m_matrixStack.pop();
    }

    return false;
  }

  return true;
}

bool TransformTraverser::preTraverse(LODNode* treeNode)
{
  treeNode->removeDirtyFlag(GroupNode::TRF_DIRTY);//transformation is updated now

  if(!m_matrixStack.empty())
  {
    treeNode->transformPosition(m_matrixStack.top());
  }
  else
  {
    treeNode->transformPosition(Mat<float, 4>::identity());
  }

  return true;
}

void TransformTraverser::postTraverse(LODNode* treeNode)
{
}

bool TransformTraverser::preTraverse(GeoNode* treeNode)
{
  if(!m_matrixStack.empty())
  {
    treeNode->setTransformationMatrix(m_matrixStack.top());
  }
  else
  {
    treeNode->setTransformationMatrix(Mat<float, 4>::identity());
  }

  return true;
}

void TransformTraverser::postTraverse(GeoNode* treeNode)
{
}

bool TransformTraverser::preTraverse(BillboardNode* treeNode)
{
  treeNode->setTransformationMatrix(m_matrixStack.top());

  return true;
}

void TransformTraverser::postTraverse(BillboardNode* treeNode)
{
}

bool TransformTraverser::preTraverse(ParticleNode* treeNode)
{
  return true;
}

void TransformTraverser::postTraverse(ParticleNode* treeNode)
{
}

bool TransformTraverser::preTraverse(LightNode* treeNode)
{
  return true;
}

void TransformTraverser::postTraverse(LightNode* treeNode)
{
}