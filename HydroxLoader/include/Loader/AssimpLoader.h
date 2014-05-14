#ifndef ASSIMPLOADER_H_
#define ASSIMPLOADER_H_

#include <stack>
#include <string>
#include <assert.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Loader/DLLExport.h"

#include <Utilities/Signals/EventManager.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Renderer/Resources/ResourceManager.hpp>

#include <SceneGraph/Scene/Scene.h>
#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>

namespace he
{
  class sg::GroupNode;
  class sg::TreeNode;
  class sg::AnimatedGeoNode;

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
      AssimpLoader(const AssimpLoader& o);

      ~AssimpLoader();

      AssimpLoader& operator=(const AssimpLoader& o);

      void setAnimationTimeUnit(AnimationTimeUnit animationTimeUnit);
      AnimationTimeUnit getAnimationTimeUnit() const;

      sg::Scene* load(std::string path, std::string filename, bool yAxisFlipped = true);

      sg::Scene* loadDefaultSceneGraph() const;

    private:

      void loadMeshesFromAssimp(std::vector<util::ResourceHandle>& out_meshes, const aiScene *scene, bool yAxisFlipped);
      util::ResourceHandle loadVertices(const aiMesh *mesh, unsigned int meshIndex, bool yAxisFlipped);
      void loadAnimatedSkeleton(const aiScene *scene);
      sg::GroupNode* loadSceneGraphFromAssimp(std::string filename, const aiScene *scene, const std::vector<util::ResourceHandle>& meshes);
      sg::TreeNode* createSceneNodes(const aiScene *scene, const aiNode *node, const std::vector<util::ResourceHandle>& meshes, sg::GroupNode *parentNode, sg::TreeNode *nextSibling);
      void attachBonesToSkinnedMesh();    

      AnimationTimeUnit m_animationTimeUnit;
      float m_animationTimeUnitConvert;

      util::SingletonManager *m_singletonManager;

      util::EventManager *m_eventManager;

      renderer::ModelManager *m_modelManager;
      renderer::MaterialManager *m_materialManager;
      renderer::TextureManager *m_textureManager;
      renderer::RenderShaderManager *m_renderShaderManager;

      util::ResourceHandle m_defaultMaterial;

      std::map<std::string, std::vector<sg::AnimationTrack>> m_animationTracks;//all the animation tracks per node
      std::vector<std::vector<util::Matrix<float, 4>>> m_inverseBindPoseTable;//all the boneMatrices per mesh
      std::vector<std::vector<std::string>> m_boneNameTable;

      std::map<sg::AnimatedGeoNode*, std::vector<std::string>> m_skinnedMeshTable;//names of the skinned geo nodes
      std::map<std::string, sg::AnimatedTransformNode*> m_boneTable;//names of the bones for skinning
    };
  }
}
#endif
