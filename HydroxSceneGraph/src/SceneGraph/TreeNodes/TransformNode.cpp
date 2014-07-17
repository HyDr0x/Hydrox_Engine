#include "SceneGraph/TreeNodes/TransformNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/ConstTraverser.h"
#include "SceneGraph/Traverser/Traverser.h"
#include "SceneGraph/Traverser/GetGlobalCoordinateTraverser.h"

namespace he
{
  namespace sg
  {
    TransformNode::TransformNode(util::Matrix<float, 4>& trfMatrix, const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling, NodeIndex firstChild) : GroupNode(nodeName, parent, nextSibling, firstChild)
    {
      util::Vector<float, 3> angles;
      util::Vector<float, 3> scale;
      util::math::decomposeMatrix(trfMatrix, angles, m_translation, scale);

      assert(abs(scale[0] - scale[1]) < 0.001f && abs(scale[0] - scale[2]) < 0.001f && abs(scale[1] - scale[2]) < 0.001f && "NON UNIFORM SCALE NOT ALLOWED");

      m_scale = scale[0];
      m_rotation = util::math::createRotXQuaternion(angles[0]) * util::math::createRotYQuaternion(angles[1]) * util::math::createRotZQuaternion(angles[2]);

      m_index.nodeType = TRANSFORMNODE;
    }

    TransformNode::TransformNode(util::Vector<float, 3>& translation, float scale, util::Quaternion<float>& rotation, const std::string& nodeName, NodeIndex parent, NodeIndex nextSibling, NodeIndex firstChild)
      : GroupNode(nodeName, parent, nextSibling, firstChild),
      m_translation(translation),
      m_scale(scale),
      m_rotation(rotation)
    {
      m_index.nodeType = TRANSFORMNODE;
    }

    TransformNode::TransformNode(const TreeNode& sourceNode) : GroupNode(sourceNode.getNodeName(), sourceNode.getParent(), sourceNode.getNextSibling(), sourceNode.getFirstChild())
    {
      assert(TRANSFORMNODE == sourceNode.getNodeType());

      const TransformNode& copyNode = static_cast<const TransformNode&>(sourceNode);

      new (this) TransformNode(copyNode);
    }

    TransformNode::~TransformNode()
    {
    }

    TreeNode& TransformNode::operator=(const TreeNode& sourceNode)
    {
      assert(m_index.nodeType == sourceNode.getNodeType());

      this->~TransformNode();

      new (this) TransformNode(sourceNode);

      return *this;
    }

    GroupNode* TransformNode::clone() const
    {
      TransformNode *newNode = new TransformNode(util::Matrix<float,4>::identity(), m_nodeName);

      newNode->m_nodeName = m_nodeName;
      newNode->m_dirtyFlag = m_dirtyFlag;

      newNode->m_translation = m_translation;
      newNode->m_scale = m_scale;
      newNode->m_rotation = m_rotation;

      return newNode;
    }

    bool TransformNode::ascendTraverse(Traverser* traverser)
    {
      return traverser->ascendTraverse(*this);
    }

    bool TransformNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(*this);
    }

    void TransformNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(*this);
    }

    bool TransformNode::ascendTraverse(ConstTraverser* traverser) const
    {
      return traverser->ascendTraverse(*this);
    }

    bool TransformNode::preTraverse(ConstTraverser* traverser) const
    {
      return traverser->preTraverse(*this);
    }

    void TransformNode::postTraverse(ConstTraverser* traverser) const
    {
      traverser->postTraverse(*this);
    }

    void TransformNode::calculateTransformation(util::Vector<float, 3>& translation, float& scale, util::Quaternion<float>& rotation) const
    {
      translation += rotation.apply(m_translation * scale);
      rotation *= m_rotation;
      scale *= m_scale;
    }

    ///////////////////TRANSFORMATIONS//////////////////////////

    //util::Vector<float, 3> TransformNode::getGlobalPosition()
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  return traverser.getGlobalTranslation() + traverser.getGlobalRotation().apply(m_translation * traverser.getGlobalScale());
    //}

    //util::Quaternion<float> TransformNode::getGlobalRotation()
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  return traverser.getGlobalRotation() * m_rotation;
    //}

    //float TransformNode::getGlobalScale()
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  return traverser.getGlobalScale() * m_scale;
    //}

    util::Vector<float, 3> TransformNode::getLocalPosition() const
    {
      return m_translation;
    }

    util::Quaternion<float> TransformNode::getLocalRotation() const
    {
      return m_rotation;
    }

    float TransformNode::getLocalScale() const
    {
      return m_scale;
    }

    //void TransformNode::setGlobalTranslation(float x, float y, float z)
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  m_translation = traverser.getGlobalRotation().invert().apply(util::Vector<float, 3>(x, y, z) - traverser.getGlobalTranslation()) / traverser.getGlobalScale();
    //}

    //void TransformNode::setGlobalTranslation(const util::Vector<float, 3>& v)
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  m_translation = traverser.getGlobalRotation().invert().apply(v - traverser.getGlobalTranslation()) / traverser.getGlobalScale();
    //}

    //void TransformNode::setGlobalRotation(const util::Quaternion<float>& q)
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  m_rotation = traverser.getGlobalRotation().invert() * q;
    //}

    //void TransformNode::setGlobalScale(float s)
    //{
    //  GetGlobalCoordinateTraverser traverser;
    //  traverser.doAscend(*this);

    //  m_scale = s / traverser.getGlobalScale();
    //}

    void TransformNode::setLocalTranslation(float x, float y, float z)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_translation[0] = x;
      m_translation[1] = y;
      m_translation[2] = z;
    }

    void TransformNode::setLocalTranslation(const util::Vector<float, 3>& v)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_translation = v;
    }

    void TransformNode::addLocalTranslation(float x, float y, float z)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_translation[0] += x;
      m_translation[1] += y;
      m_translation[2] += z;
    }

    void TransformNode::addLocalTranslation(const util::Vector<float, 3>& v)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_translation += v;
    }

    void TransformNode::setLocalRotation(const util::Quaternion<float>& q)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = q;
    }

    void TransformNode::addLocalRotation(const util::Quaternion<float>& q)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation *= q;
    }

    void TransformNode::setLocalRotationX(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = util::math::createRotXQuaternion(angle);
    }

    void TransformNode::addLocalRotationX(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation *= util::math::createRotXQuaternion(angle);
    }

    void TransformNode::setLocalRotationY(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = util::math::createRotYQuaternion(angle);
    }

    void TransformNode::addLocalRotationY(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation *= util::math::createRotYQuaternion(angle);
    }

    void TransformNode::setLocalRotationZ(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = util::math::createRotZQuaternion(angle);
    }

    void TransformNode::addLocalRotationZ(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation *= util::math::createRotZQuaternion(angle);
    }

    void TransformNode::setLocalRotationXYZ(const util::Vector<float, 3>& angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = util::math::createRotXQuaternion(angle[0]) * util::math::createRotYQuaternion(angle[1]) * util::math::createRotZQuaternion(angle[2]);
    }

    void TransformNode::addLocalRotationXYZ(const util::Vector<float, 3>& angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation *= util::math::createRotXQuaternion(angle[0]) * util::math::createRotYQuaternion(angle[1]) * util::math::createRotZQuaternion(angle[2]);
    }

    void TransformNode::setLocalRotationAxis(float angle, const util::Vector<float, 3>& axis)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = util::math::createRotAxisQuaternion(angle, axis);
    }

    void TransformNode::addLocalRotationAxis(float angle, const util::Vector<float, 3>& axis)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation *= util::math::createRotAxisQuaternion(angle, axis);
    }

    void TransformNode::setLocalScale(float s)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_scale = s;
    }

    void TransformNode::addLocalScale(float s)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_scale += s;
    }
  }
}