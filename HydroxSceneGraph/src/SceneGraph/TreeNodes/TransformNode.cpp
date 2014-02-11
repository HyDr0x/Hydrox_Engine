#include "SceneGraph/TreeNodes/TransformNode.h"

#include <assert.h>

#include "SceneGraph/Traverser/Traverser.h"

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

    util::Vector<float, 3> TransformNode::getPosition()
    {
	    return m_translation;
    }

    util::Quaternion<float> TransformNode::getRotation()
    {
	    return m_rotation;
    }

    float TransformNode::getScale()
    {
	    return m_scale;
    }

    void TransformNode::setTranslation(float x, float y, float z)
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

    void TransformNode::setTranslation(util::Vector<float, 3> v)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_translation = v;
    }

    void TransformNode::addTranslation(float x, float y, float z)
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

    void TransformNode::addTranslation(util::Vector<float, 3> v)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_translation += v;
    }

    void TransformNode::setRotation(util::Quaternion<float> q)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = q;
    }

    void TransformNode::addRotation(util::Quaternion<float> q)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation *= q;
    }

    void TransformNode::setRotationX(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation = util::math::createRotXQuaternion(angle);
    }

    void TransformNode::addRotationX(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation *= util::math::createRotXQuaternion(angle);
    }

    void TransformNode::setRotationY(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation = util::math::createRotYQuaternion(angle);
    }

    void TransformNode::addRotationY(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation *= util::math::createRotYQuaternion(angle);
    }

    void TransformNode::setRotationZ(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation = util::math::createRotZQuaternion(angle);
    }

    void TransformNode::addRotationZ(float angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation *= util::math::createRotZQuaternion(angle);
    }

    void TransformNode::setRotationXYZ(util::Vector<float, 3> angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation = util::math::createRotXQuaternion(angle[0]) * util::math::createRotYQuaternion(angle[1]) * util::math::createRotZQuaternion(angle[2]);
    }

    void TransformNode::addRotationXYZ(util::Vector<float, 3> angle)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_rotation *= util::math::createRotXQuaternion(angle[0]) * util::math::createRotYQuaternion(angle[1]) * util::math::createRotZQuaternion(angle[2]);
    }

    void TransformNode::setRotationAxis(float angle, util::Vector<float, 3> axis)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation = util::math::createRotAxisQuaternion(angle, axis);
    }

    void TransformNode::addRotationAxis(float angle, util::Vector<float, 3> axis)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

      m_rotation *= util::math::createRotAxisQuaternion(angle, axis);
    }

    void TransformNode::setScale(float s)
    {
      if(!m_dirtyFlag & TRF_DIRTY)//add it only if its not dirty already
      {
        notify(this);
        addDirtyFlag(TRF_DIRTY);
      }

	    m_scale = s;
    }

    void TransformNode::addScale(float s)
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