#include "SceneGraph/Traverser/GetGlobalCoordinateTraverser.h"

#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"

namespace he
{
  namespace sg
  {    GetGlobalCoordinateTraverser::GetGlobalCoordinateTraverser()
    {
    }

    GetGlobalCoordinateTraverser::~GetGlobalCoordinateTraverser()
    {
    }

    void GetGlobalCoordinateTraverser::clearStacks()
    {
      while(!m_scaleStack.empty())
      {
        m_translateStack.pop();
        m_scaleStack.pop();
        m_rotationStack.pop();
      }
    }

    void GetGlobalCoordinateTraverser::postAscendTraverse()
    {
      util::Vector<float, 3> trfTranslation = util::Vector<float, 3>::identity();
      float trfScale = 1.0f;
      util::Quaternion<float> trfRotation = util::Quaternion<float>::identity();

      while(!m_scaleStack.empty())//clear the matrix stack, cause there is another dirty node higher in the tree
      {
        util::Vector<float, 3> translation = m_translateStack.top();
        float scale = m_scaleStack.top();
        util::Quaternion<float> rotation = m_rotationStack.top();

        m_translateStack.pop();
        m_scaleStack.pop();
        m_rotationStack.pop();

        trfTranslation += trfRotation.apply(translation * trfScale);
        trfScale *= scale;
        trfRotation *= rotation;
      }

      m_globalTranslation = trfTranslation;
      m_globalScale = trfScale;
      m_globalRotation = trfRotation;
    }

    bool GetGlobalCoordinateTraverser::ascendTraverse(const AnimatedTransformNode* treeNode)
    {
      m_translateStack.push(treeNode->getLocalPosition());
      m_scaleStack.push(treeNode->getLocalScale());
      m_rotationStack.push(treeNode->getLocalRotation());

      return true;
    }

    bool GetGlobalCoordinateTraverser::ascendTraverse(const TransformNode* treeNode)
    {
      m_translateStack.push(treeNode->getLocalPosition());
      m_scaleStack.push(treeNode->getLocalScale());
      m_rotationStack.push(treeNode->getLocalRotation());

      return true;
    }

    float GetGlobalCoordinateTraverser::getGlobalScale() const
    {
      return m_globalScale;
    }

    util::Vector<float, 3> GetGlobalCoordinateTraverser::getGlobalTranslation() const
    {
      return m_globalTranslation;
    }

    util::Quaternion<float> GetGlobalCoordinateTraverser::getGlobalRotation() const
    {
      return m_globalRotation;
    }
  }
}