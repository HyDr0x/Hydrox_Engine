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

void TransformTraverser::postAscendTraverse()
{
  Vec<float, 3> trfTranslation = Vec<float, 3>(0.0f, 0.0f, 0.0f);
  Vec<float, 3> trfScale = Vec<float, 3>(1.0f, 1.0f, 1.0f);
  Quaternion<float> trfRotation = Quaternion<float>::identity();

  while(!m_scaleStack.empty())//clear the matrix stack, cause there is another dirty node higher in the tree
  {
    Vec<float, 3> translation = m_translateStack.top();
    Vec<float, 3> scale = m_scaleStack.top();
    Quaternion<float> rotation = m_rotationStack.top();

    m_translateStack.pop();
    m_scaleStack.pop();
    m_rotationStack.pop();

    trfTranslation += trfRotation.apply(trfScale * translation);
    trfScale *= scale;
    trfRotation *= rotation;
  }

  m_translateStack.push(trfTranslation);
  m_scaleStack.push(trfScale);
  m_rotationStack.push(trfRotation);
}

bool TransformTraverser::ascendTraverse(TransformNode* treeNode)
{
  if(!treeNode->getDirtyFlag() & GroupNode::TRF_DIRTY)
  {
    Vec<float, 3> translation = treeNode->getPosition();
    Vec<float, 3> scale = treeNode->getScale();
    Quaternion<float> rotation = treeNode->getRotation();

    m_translateStack.push(translation);
    m_scaleStack.push(scale);
    m_rotationStack.push(rotation);

    return true;
  }

  while(!m_scaleStack.empty())//clear the matrix stack, cause there is another dirty node higher in the tree
  {
    m_translateStack.pop();
    m_scaleStack.pop();
    m_rotationStack.pop();
  }

  return false;
}

bool TransformTraverser::preTraverse(TransformNode* treeNode)
{
  treeNode->removeDirtyFlag(GroupNode::TRF_DIRTY);//transformation is updated now

  Vec<float, 3> translation;
  Vec<float, 3> scale;
  Quaternion<float> rotation;

  if(!m_scaleStack.empty())
  {
    translation = m_translateStack.top();
    scale = m_scaleStack.top();
    rotation = m_rotationStack.top();
  }
  else
  {
    translation = Vec<float, 3>(0.0f, 0.0f, 0.0f);
    scale = Vec<float, 3>(1.0f, 1.0f, 1.0f);
    rotation = Quaternion<float>::identity();
  }

  treeNode->calculateTransformation(translation, scale, rotation);

  m_translateStack.push(translation);
  m_scaleStack.push(scale);
  m_rotationStack.push(rotation);

  return true;
}

void TransformTraverser::postTraverse(TransformNode* treeNode)
{
  m_scaleStack.pop();
  m_translateStack.pop();
  m_rotationStack.pop();
}

bool TransformTraverser::ascendTraverse(LODNode* treeNode)
{
  if(treeNode->getDirtyFlag() & GroupNode::TRF_DIRTY)
  {
    while(!m_scaleStack.empty())//clear the matrix stack, cause there is another dirty node higher in the tree
    {
      m_translateStack.pop();
      m_scaleStack.pop();
      m_rotationStack.pop();
    }

    return false;
  }

  return true;
}

bool TransformTraverser::preTraverse(LODNode* treeNode)
{
  treeNode->removeDirtyFlag(GroupNode::TRF_DIRTY);//transformation is updated now

  if(!m_scaleStack.empty())
  {
    treeNode->transformPosition(m_translateStack.top(), m_scaleStack.top(), m_rotationStack.top());
  }
  else
  {
    treeNode->transformPosition(m_translateStack.top(), m_scaleStack.top(), Quaternion<float>::identity());
  }

  return true;
}

void TransformTraverser::postTraverse(LODNode* treeNode)
{
}

bool TransformTraverser::preTraverse(GeoNode* treeNode)
{
  treeNode->setTransformationMatrix(math::createTransformationMatrix(m_translateStack.top(), m_scaleStack.top(), m_rotationStack.top()));

  return true;
}

void TransformTraverser::postTraverse(GeoNode* treeNode)
{
}

bool TransformTraverser::preTraverse(BillboardNode* treeNode)
{
  treeNode->setTransformationMatrix(math::createTransformationMatrix(m_translateStack.top(), m_scaleStack.top(), m_rotationStack.top()));

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