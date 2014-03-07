#include "SceneGraph/TreeNodes/TransformNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"
#include "SceneGraph/Traverser/GetGlobalCoordinateTraverser.h"

namespace he
{
	namespace sg
	{
    TransformNode::TransformNode(util::Matrix<float, 4>& trfMatrix, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild) : GroupNode(nodeName, parent, nextSibling, firstChild)
    {
      util::Vector<float, 3> angles;
      util::Vector<float, 3> scale;
      util::math::decomposeMatrix(trfMatrix, angles, m_translation, scale);

      assert(abs(scale[0] - scale[1]) < 0.001f && abs(scale[0] - scale[2]) < 0.001f && abs(scale[1] - scale[2]) < 0.001f && "NON UNIFORM SCALE NOT ALLOWED");

      m_scale = scale[0];
      m_rotation = util::math::createRotXQuaternion(angles[0]) * util::math::createRotYQuaternion(angles[1]) * util::math::createRotZQuaternion(angles[2]);
    }

    TransformNode::TransformNode(util::Vector<float, 3>& translation, float scale, util::Quaternion<float>& rotation, const std::string& nodeName, GroupNode* parent, TreeNode* nextSibling, TreeNode* firstChild)
      : GroupNode(nodeName, parent, nextSibling, firstChild),
      m_translation(translation),
      m_scale(scale),
      m_rotation(rotation)
    {
    }

    TransformNode& TransformNode::operator=(const TransformNode& sourceNode)
    {
      GroupNode::operator=(sourceNode);

      m_translation = sourceNode.m_translation;
      m_scale = sourceNode.m_scale;
      m_rotation = sourceNode.m_rotation;

      return *this;
    }

    TreeNode& TransformNode::operator=(const TreeNode& sourceNode)
    {
      assert(typeid(*this) == typeid(sourceNode));

      const TransformNode& copyNode = static_cast<const TransformNode&>(sourceNode);
      TransformNode::operator=(copyNode);

      return *this;
    }

    TransformNode::~TransformNode()
    {
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
      return traverser->ascendTraverse(this);
    }

    bool TransformNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void TransformNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    bool TransformNode::isTransformNode()
    {
      return true;
    }

    void TransformNode::calculateTransformation(util::Vector<float, 3>& translation, float& scale, util::Quaternion<float>& rotation)
    {
      translation += rotation.apply(m_translation * scale);
      rotation *= m_rotation;
      scale *= m_scale;
    }

    ///////////////////TRANSFORMATIONS//////////////////////////

    util::Vector<float, 3> TransformNode::getGlobalPosition()
    {
      GetGlobalCoordinateTraverser traverser;
      traverser.doAscend(this);

      return traverser.getGlobalTranslation() + traverser.getGlobalRotation().apply(m_translation * traverser.getGlobalScale());
    }

    util::Quaternion<float> TransformNode::getGlobalRotation()
    {
      GetGlobalCoordinateTraverser traverser;
      traverser.doAscend(this);

      return traverser.getGlobalRotation() * m_rotation;
    }

	  float TransformNode::getGlobalScale()
    {
      GetGlobalCoordinateTraverser traverser;
      traverser.doAscend(this);

      return traverser.getGlobalScale() * m_scale;
    }

    util::Vector<float, 3> TransformNode::getLocalPosition()
    {
	    return m_translation;
    }

    util::Quaternion<float> TransformNode::getLocalRotation()
    {
	    return m_rotation;
    }

    float TransformNode::getLocalScale()
    {
	    return m_scale;
    }

    void TransformNode::setGlobalTranslation(float x, float y, float z)
    {
      GetGlobalCoordinateTraverser traverser;
      traverser.doAscend(this);

      m_translation = traverser.getGlobalRotation().invert().apply(util::Vector<float, 3>(x, y, z) - traverser.getGlobalTranslation()) / traverser.getGlobalScale();
    }

    void TransformNode::setGlobalTranslation(util::Vector<float, 3> v)
    {
      GetGlobalCoordinateTraverser traverser;
      traverser.doAscend(this);

      m_translation = traverser.getGlobalRotation().invert().apply(v - traverser.getGlobalTranslation()) / traverser.getGlobalScale();
    }

    void TransformNode::setGlobalRotation(util::Quaternion<float> q)
    {
      GetGlobalCoordinateTraverser traverser;
      traverser.doAscend(this);

      m_rotation = traverser.getGlobalRotation().invert() * q;
    }

    void TransformNode::setGlobalScale(float s)
    {
      GetGlobalCoordinateTraverser traverser;
      traverser.doAscend(this);

      m_scale = s / traverser.getGlobalScale();
    }

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

    void TransformNode::setLocalTranslation(util::Vector<float, 3> v)
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

    void TransformNode::addLocalTranslation(util::Vector<float, 3> v)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_translation += v;
    }

    void TransformNode::setLocalRotation(util::Quaternion<float> q)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = q;
    }

    void TransformNode::addLocalRotation(util::Quaternion<float> q)
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

    void TransformNode::setLocalRotationXYZ(util::Vector<float, 3> angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation = util::math::createRotXQuaternion(angle[0]) * util::math::createRotYQuaternion(angle[1]) * util::math::createRotZQuaternion(angle[2]);
    }

    void TransformNode::addLocalRotationXYZ(util::Vector<float, 3> angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation *= util::math::createRotXQuaternion(angle[0]) * util::math::createRotYQuaternion(angle[1]) * util::math::createRotZQuaternion(angle[2]);
    }

    void TransformNode::setLocalRotationAxis(float angle, util::Vector<float, 3> axis)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = util::math::createRotAxisQuaternion(angle, axis);
    }

    void TransformNode::addLocalRotationAxis(float angle, util::Vector<float, 3> axis)
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