#ifndef READNODESTREAMING_H_
#define READNODESTREAMING_H_

#include <fstream>
#include <map>

#include <Utilities/Miscellaneous/ResourceHandle.h>

namespace he
{
  namespace util
  {
    class EventManager;
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

  namespace loader
  {
    void read(std::istream& stream, sg::TreeNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::GeoNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::AnimatedGeoNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::TransformNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::AnimatedTransformNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::LODNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::BillboardNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::LightNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::ShadowLightNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::ParticleNode& node, util::EventManager *eventManager, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles);
    void read(std::istream& stream, sg::TreeNodeAllocator& allocator, std::map<std::string, std::map<unsigned int, util::ResourceHandle>> resourceHandles, util::EventManager *eventManager);
  }
}

#endif