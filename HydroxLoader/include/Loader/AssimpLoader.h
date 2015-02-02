#ifndef ASSIMPLOADER_H_
#define ASSIMPLOADER_H_

#include <stack>
#include <string>
#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Loader/DLLExport.h"

#include <Utilities/Signals/EventManager.h>
#include <Utilities/Miscellaneous/CacheGenerator.h>

#include <DataBase/ResourceManager.hpp>

#include <SceneGraph/Scene/Scene.h>
#include <SceneGraph/Scene/TreeNodeAllocator.h>
#include <SceneGraph/TreeNodes/TreeNode.h>
#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace sg
  {
    class GroupNode;
    class TreeNode;
    class AnimatedGeoNode;
  }

  namespace loader
  {
    class GRAPHICAPI AssimpLoader
    {
    public:

      enum AnimationTimeUnit
      {
        Microseconds,
        Milliseconds,
        Seconds,
        Minutes,
      };

      AssimpLoader(util::SingletonManager *singletonManager);
      AssimpLoader(const AssimpLoader& other);

      ~AssimpLoader();

      AssimpLoader& operator=(const AssimpLoader& other);

      void setCacheGeneratorParamter(float errorRate, float maxDistance, float maxAngle);

      void setAnimationTimeUnit(AnimationTimeUnit animationTimeUnit);
      AnimationTimeUnit getAnimationTimeUnit() const;

      sg::Scene* load(std::string path, std::string filename, bool yAxisFlipped = true);

      sg::Scene* loadDefaultSceneGraph();

    private:

      void loadMeshesFromAssimp(const aiScene *scene, bool yAxisFlipped);
      util::ResourceHandle loadVertices(const aiMesh *mesh, unsigned int meshIndex, bool yAxisFlipped);

      void loadAnimatedSkeleton(const aiScene *scene);
      

      sg::NodeIndex loadSceneGraphFromAssimp(std::string filename, const aiScene *scene);
      sg::NodeIndex createTransformNodes(const aiNode *node, sg::NodeIndex parentNode, sg::NodeIndex nextSibling);
      sg::NodeIndex createGeoNodes(std::string meshName, unsigned int meshIndex, sg::NodeIndex parentNode, sg::NodeIndex nextSibling);

      void attachBonesToSkinnedMesh();

      void loadMaterialsFromAssimp(std::string path, const aiScene *scene);

      float m_errorRate;
      float m_maxDistance;
      float m_maxAngle;

      AnimationTimeUnit m_animationTimeUnit;
      float m_animationTimeUnitConvert;

      util::SingletonManager *m_singletonManager;

      util::SharedPointer<util::EventManager> m_eventManager;

      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<db::MaterialManager> m_materialManager;
      util::SharedPointer<db::TextureManager> m_textureManager;
      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;

      util::CacheGenerator m_generator;

      util::ResourceHandle m_defaultMaterial;

      sg::TreeNodeAllocator m_allocator;

      std::map<std::string, std::vector<sg::AnimationTrack>> m_animationTracks;//all the animation tracks per node
      std::vector<std::vector<util::Matrix<float, 4>>> m_inverseBindPoseTable;//all the boneMatrices per mesh
      std::vector<std::vector<std::string>> m_boneNameTable;
      std::vector<util::ResourceHandle> m_meshes;//contains the mesh id's of the scene
      std::vector<unsigned int> m_materialIndex;//the index into the material array
      std::vector<util::ResourceHandle> m_materials;//contains the material id's of the scene

      std::map<sg::NodeIndex, std::vector<std::string>, sg::NodeIndex::Less> m_skinnedMeshTable;//names of the skinned geo nodes
      std::map<std::string, sg::NodeIndex> m_boneTable;//names of the bones for skinning
    };
  }
}
#endif
