#ifndef NODEEXTRACTIONTRAVERSER_H_
#define NODEEXTRACTIONTRAVERSER_H_

#include <string>
#include <vector>

#include <DataBase/ResourceManager.hpp>
#include <SceneGraph/Traverser/Traverser.h>
#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace sg
  {
    class TransformNode;
    class AnimatedTransformNode;
    class GeoNode;
    class AnimatedGeoNode;
    class LODNode;
    class BillboardNode;
    class LightNode;
    class ShadowLightNode;
    class ParticleNode;
  }

  namespace util
  {
    class SingletonManager;
  }

  namespace saver
  {
    struct NodeWrapperMapper;

    class NodeExtractionTraverser : public sg::Traverser
    {
    public:

      NodeExtractionTraverser(std::string fileName, NodeWrapperMapper& wrapperMapper, util::SingletonManager *singletonManager);
      virtual ~NodeExtractionTraverser();

      virtual bool preTraverse(sg::TransformNode* treeNode);

      virtual bool preTraverse(sg::AnimatedTransformNode* treeNode);

      virtual bool preTraverse(sg::GeoNode* treeNode);

      virtual bool preTraverse(sg::AnimatedGeoNode* treeNode);

      virtual bool preTraverse(sg::BillboardNode* treeNode);

      virtual bool preTraverse(sg::LODNode* treeNode);

      virtual bool preTraverse(sg::LightNode* treeNode);

      virtual bool preTraverse(sg::ShadowLightNode* treeNode);

      virtual bool preTraverse(sg::ParticleNode* treeNode);

    protected:

      std::string m_fileName;

      NodeWrapperMapper& m_wrapperMapper;

      db::ModelManager *m_modelManager;
      db::MaterialManager *m_materialManager;
      db::RenderShaderManager *m_renderShaderManager;
      db::TextureManager *m_textureManager;
    };
  }
}

#endif
