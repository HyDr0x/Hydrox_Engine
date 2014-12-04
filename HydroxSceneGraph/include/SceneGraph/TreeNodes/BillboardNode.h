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
  {
    class Traverser;

    class GRAPHICAPI BillboardNode : public TreeNode
    {
    public:

      BillboardNode(){}
      BillboardNode(util::SharedPointer<util::EventManager> eventManager, util::ResourceHandle textureHandle, util::vec2ui animNumber, util::vec2f texStart, util::vec2f texEnd,
        const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0);
      BillboardNode(const BillboardNode& sourceNode);
      BillboardNode(const TreeNode& sourceNode);
      virtual ~BillboardNode();

      virtual TreeNode& operator=(const TreeNode& other);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      void setTextureHandle(util::ResourceHandle textureHandle);
      util::ResourceHandle getTextureHandle() const;

      void setCurrentAnimationFrame(unsigned int currentFrame);
      void setCurrentAnimationFrame(const util::vec2ui& currentFrame);
      util::vec2ui getCurrentAnimationFrame() const;

      void setAnimationNumber(const util::vec2ui& animationNumber);
      util::vec2ui getAnimationNumber() const;

      void setTranslation(const util::vec3f& v);
      void setTranslation(float x, float y, float z);
      void addTranslation(const util::vec3f& v);
      void addTranslation(float x, float y, float z);
  
      void setScale(float s);
      void addScale(float s);

      void setScale(const util::vec2f& s);
      void setScale(float sx, float sy);
      void addScale(const util::vec2f& s);
      void addScale(float sx, float sy);

      util::vec3f getPosition() const;
      util::vec2f getScale() const;

      void setTextureStart(const util::vec2f& texStart);
      util::vec2f getTextureStart() const;

      void setTextureEnd(const util::vec2f& texEnd);
      util::vec2f getTextureEnd() const;

      util::Matrix<float, 3> getTexTransformationMatrix() const;

      void setRenderable(bool renderable);
      bool getRenderable() const;

      virtual void read(std::istream& stream, util::SharedPointer<util::EventManager> eventManager, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles);
      virtual void write(std::ostream& stream, const std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceHandles) const;

    private:

      util::SharedPointer<util::EventManager> m_eventManager;

      util::vec2ui m_animNumber;
      util::vec2ui m_currentFrame;
      util::vec2f m_texStart;
      util::vec2f m_texEnd;

      util::vec2f m_scale;
      util::vec3f m_translate;

      util::ResourceHandle m_textureHandle;

      bool m_renderable;//boolean which decides if the mesh is being drawn or not
    };
  }
}

#endif
