#include "SceneGraph/Traverser/TransformTraverser.h"

#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"

namespace he
{
  namespace sg
  {
    TransformTraverser::TransformTraverser()
    {
    }

    TransformTraverser::~TransformTraverser()
    {
    }

    void TransformTraverser::clearStacks()
    {
      while(!m_scaleStack.empty())
      {
        m_translateStack.pop();
        m_scaleStack.pop();
        m_rotationStack.pop();
      }
    }

    void TransformTraverser::postAscendTraverse()
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

      m_translateStack.push(trfTranslation);
      m_scaleStack.push(trfScale);
      m_rotationStack.push(trfRotation);
    }

    bool TransformTraverser::ascendTraverse(AnimatedTransformNode* treeNode)
    {
      if(!(treeNode->getDirtyFlag() & GroupNode::TRF_DIRTY || treeNode->getDirtyFlag() & GroupNode::ANIM_DIRTY))
      {
        m_translateStack.push(treeNode->getLocalPosition());
        m_scaleStack.push(treeNode->getLocalScale());
        m_rotationStack.push(treeNode->getLocalRotation());

        return true;
      }

      clearStacks();//clear the transform stack, because there is another dirty node higher in the tree

      return false;
    }

    bool TransformTraverser::preTraverse(AnimatedTransformNode* treeNode)
    {
      util::Vector<float, 3> translation;
      float scale;
      util::Quaternion<float> rotation;

      if(!m_scaleStack.empty())
      {
        translation = m_translateStack.top();
        scale = m_scaleStack.top();
        rotation = m_rotationStack.top();
      }
      else
      {
        translation = util::Vector<float, 3>::identity();
        scale = 1.0f;
        rotation = util::Quaternion<float>::identity();
      }

      treeNode->calculateTransformation(translation, scale, rotation);

      m_translateStack.push(translation);
      m_scaleStack.push(scale);
      m_rotationStack.push(rotation);

      treeNode->removeDirtyFlag((GroupNode::DirtyFlags)(GroupNode::ANIM_DIRTY | GroupNode::TRF_DIRTY));//animation is updated now

      return true;
    }

    void TransformTraverser::postTraverse(AnimatedTransformNode* treeNode)
    {
      m_scaleStack.pop();
      m_translateStack.pop();
      m_rotationStack.pop();
    }

    bool TransformTraverser::ascendTraverse(TransformNode* treeNode)
    {
      if(!(treeNode->getDirtyFlag() & GroupNode::TRF_DIRTY || treeNode->getDirtyFlag() & GroupNode::ANIM_DIRTY))
      {
        m_translateStack.push(treeNode->getLocalPosition());
        m_scaleStack.push(treeNode->getLocalScale());
        m_rotationStack.push(treeNode->getLocalRotation());

        return true;
      }

      clearStacks();//clear the transform stack, because there is another dirty node higher in the tree

      return false;
    }

    bool TransformTraverser::preTraverse(TransformNode* treeNode)
    {
      util::Vector<float, 3> translation;
      float scale;
      util::Quaternion<float> rotation;

      if(!m_scaleStack.empty())
      {
        translation = m_translateStack.top();
        scale = m_scaleStack.top();
        rotation = m_rotationStack.top();
      }
      else
      {
        translation = util::Vector<float, 3>::identity();
        scale = 1.0f;
        rotation = util::Quaternion<float>::identity();
      }

      treeNode->calculateTransformation(translation, scale, rotation);

      treeNode->removeDirtyFlag(GroupNode::TRF_DIRTY);//transformation is updated now

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
        treeNode->transformPosition(m_translateStack.top(), m_scaleStack.top(), util::Quaternion<float>::identity());
      }

      return true;
    }

    void TransformTraverser::postTraverse(LODNode* treeNode)
    {
    }

    bool TransformTraverser::preTraverse(AnimatedGeoNode* treeNode)
    {
      treeNode->setTransformationMatrix(util::math::createTransformationMatrix(m_translateStack.top(), m_scaleStack.top(), m_rotationStack.top()));

      return true;
    }

    void TransformTraverser::postTraverse(AnimatedGeoNode* treeNode)
    {
    }

    bool TransformTraverser::preTraverse(GeoNode* treeNode)
    {
      treeNode->setTransformationMatrix(util::math::createTransformationMatrix(m_translateStack.top(), m_scaleStack.top(), m_rotationStack.top()));

      return true;
    }

    void TransformTraverser::postTraverse(GeoNode* treeNode)
    {
    }

    bool TransformTraverser::preTraverse(BillboardNode* treeNode)
    {
      treeNode->setTranslation(m_translateStack.top());
      treeNode->setScale(m_scaleStack.top());

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
      treeNode->applyTransformation(m_translateStack.top(), m_rotationStack.top());

      return true;
    }

    void TransformTraverser::postTraverse(LightNode* treeNode)
    {
    }
  }
}