#ifndef BILLBOARDNODE_H_
#define BILLBOARDNODE_H_

#include "SceneGraph/DLLExport.h"

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Signals/EventManager.h>

#include "SceneGraph/TreeNodes/TreeNode.h"

namespace he
{
	namespace sg
	{    class Traverser;

    class GRAPHICAPI BillboardNode : public TreeNode
    {
    public:

      BillboardNode(util::EventManager *eventManager, util::ResourceHandle textureHandle, util::Vector<unsigned int, 2> animNumber, util::Vector<float, 2> texStart, util::Vector<float, 2> texEnd, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      BillboardNode& operator=(const BillboardNode& o);
      virtual TreeNode& operator=(const TreeNode& o);
      virtual ~BillboardNode();

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool isBillboardNode();

      util::ResourceHandle getTextureHandle() const;

      void setAnimation(unsigned int number);
	    void setAnimation(util::Vector<unsigned int, 2> number);
	    util::Vector<unsigned int, 2> getAnimationNumber();
	    util::Vector<unsigned int, 2> getAnimationCount();

	    void setTranslation(util::Vector<float, 3> v);
	    void setTranslation(float x, float y, float z);
	    void addTranslation(util::Vector<float,3> v);
	    void addTranslation(float x, float y, float z);
	
	    void setScale(float s);
	    void addScale(float s);

	    void setScale(util::Vector<float, 2> s);
	    void setScale(float sx, float sy);
	    void addScale(util::Vector<float, 2> s);
	    void addScale(float sx, float sy);

	    util::Vector<float, 3> getPosition();
	    util::Vector<float, 2> getScale();

	    util::Matrix<float, 3> getTexTransformationMatrix();
	    util::ResourceHandle getTextureID() const;

      void setRenderable(bool renderable);
	    bool getRenderable() const;

    private:

      util::EventManager *m_eventManager;

      util::Vector<unsigned int, 2> m_animNumber;
	    util::Vector<unsigned int, 2> m_animCount;
	    util::Vector<float, 2> m_texStart;
	    util::Vector<float, 2> m_texEnd;

	    util::Vector<float, 2> m_scale;
	    util::Vector<float, 3> m_translate;

      util::ResourceHandle m_textureHandle;

      bool m_renderable;//boolean which decides if the mesh is being drawn or not
    };
	}
}

#endif
