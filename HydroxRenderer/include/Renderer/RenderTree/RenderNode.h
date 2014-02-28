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

#include "Renderer/RenderTree/TreeNode.h"

namespace he
{
	namespace renderer
	{
    class Traverser;

    struct DrawElementsIndirectCommand
    {
      GLuint count;//number of indices
      GLuint instanceCount;//the number of instances
      GLuint firstIndex;//offset in the index array
      GLuint baseVertex;//offset which adds to every index value
      GLuint baseInstance;//base instance, getting added to all vertex attribute divisors, not to gl_InstanceID!
    };

    class RenderNode : public TreeNode
    {
    public:

      RenderNode(unsigned int maxMaterials, unsigned int maxGeometry);
      virtual ~RenderNode() = 0;

      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual TreeNode* createNewNode(InsertGeometryTraverser* traverser);

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle frustumCullingShaderHandle);

      bool insertGeometry(sg::GeoNode *node);
      bool removeGeometry(sg::GeoNode *node);

      void updateBuffer();

      void frustumCulling();
      void rasterizeGeometry();

    protected:

      virtual GPUBuffer& getTransformationMatrixBuffer() = 0;

      void resizeBuffer();

      virtual void resizeMatrixBuffer() = 0;

      virtual void fillMatrixBuffer(sg::GeoNode *node, unsigned int geometryIndex) = 0;
      virtual void fillMatrixBuffer(sg::AnimatedGeoNode *node, unsigned int geometryIndex) = 0;

      virtual void uploadMatrices() = 0;

      MaterialManager *m_materialManager;
      ModelManager *m_modelManager;

      FrustumCullingGPU m_frustumCulling;

      //gpu buffer for drawing

      //per mesh buffer
      GPUBuffer m_commandBuffer;
      GPUBuffer m_meshIndexBuffer;
      GPUBuffer m_meshVertexBuffer;
      GPUBuffer m_bboxesBuffer;
      GPUBuffer m_cullingCommandBuffer;

      //per instance buffer
      UBO m_materialBuffer;
      GPUBuffer m_materialIndexBuffer;
      GPUBuffer m_meshInstanceIndexBuffer;
      GPUBuffer m_matrixBuffer;
      std::vector<GLubyte> m_matrixCache;

      unsigned int m_primitiveType;

      unsigned int m_maxMaterials;
      unsigned int m_materialCount;
      unsigned int m_maxGeometry;
      unsigned int m_instanceCounter;
      unsigned int m_vboSize;
      unsigned int m_vertexStride;
      unsigned int m_indexSize;

      class Less
      {
      public:
        bool operator()(const util::ResourceHandle& o1, const util::ResourceHandle& o2) const
        {
          return o1.getID() < o2.getID();
        }
      };

      std::map<unsigned int, unsigned int> m_materialHandles;
      std::map<util::ResourceHandle, std::list<sg::GeoNode*>, Less> m_meshHandles;

      bool m_geometryChanged;
      bool m_instanceNumberChanged;
    };
	}
}

#endif
