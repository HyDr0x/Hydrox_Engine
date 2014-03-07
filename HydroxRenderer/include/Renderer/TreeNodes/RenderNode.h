#ifndef RENDERNODE_H_
#define RENDERNODE_H_

#include <list>
#include <map>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>

#include "Renderer/Resources/ResourceManager.hpp"

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/Pipeline/FrustumCullingGPU.h"

#include "Renderer/TreeNodes/TreeNode.h"

namespace he
{
	namespace renderer
	{
    class Traverser;
    class DrawCommandInterface;
    class GeometryCommandInterface;

    class RenderNode : public TreeNode
    {
    public:

      RenderNode(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones);
      virtual ~RenderNode();

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      void initialize(sg::GeoNode *node, util::SingletonManager *singletonManager, util::ResourceHandle frustumCullingShaderHandle);

      bool insertGeometry(sg::GeoNode *node);
      bool removeGeometry(sg::GeoNode *node);

      bool isEmpty();

      void frustumCulling();
      void rasterizeGeometry();

    protected:

      void updateBuffer();
      void resizeBuffer();

      MaterialManager *m_materialManager;
      ModelManager *m_modelManager;

      GeometryCommandInterface *m_geometryCommand;
      DrawCommandInterface *m_drawCommand;

      FrustumCullingGPU m_frustumCulling;

      //per instance buffer
      UBO m_materialBuffer;
      GPUBuffer m_materialIndexBuffer;
      GPUBuffer m_meshInstanceIndexBuffer;

      GLenum m_primitiveType;

      unsigned int m_maxMaterials;
      unsigned int m_materialCount;

      unsigned int m_maxGeometry;
      unsigned int m_instanceNumber;

      unsigned int m_maxBones;

      unsigned int m_vboSize;
      unsigned int m_iboSize;

      bool m_geometryChanged;
      bool m_instanceNumberChanged;

      class Less
      {
      public:
        inline bool operator()(const util::ResourceHandle& o1, const util::ResourceHandle& o2) const
        {
          return o1.getID() < o2.getID();
        }
      };

      std::map<unsigned int, unsigned int> m_materialHandles;
      std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less> m_meshHandles;
    };
	}
}

#endif
