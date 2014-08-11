#ifndef WRITENODESTREAMING_H_
#define WRITENODESTREAMING_H_

#include <fstream>

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace sg
  {
    class TreeNodeAllocator;
    class TreeNode;
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

  namespace saver
  {
    void write(std::ostream& stream, const sg::TreeNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::TransformNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::AnimatedTransformNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::GeoNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::AnimatedGeoNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::LODNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::BillboardNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::LightNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::ShadowLightNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::ParticleNode& node, util::SingletonManager *singletonManager);
    void write(std::ostream& stream, const sg::TreeNodeAllocator& allocator, util::SingletonManager *singletonManager);
  }
}

#endif