#ifndef ASSIMPLOADER_H_
#define ASSIMPLOADER_H_

#include <stack>
#include <string>
#include <assert.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Graphics/Mesh.h"

#include "Hydrox/Singletons/CacheManager.hpp"

#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"

namespace he
{
  class ILDevilLoader;

  class GroupNode;
  class TreeNode;

  class Scene;

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

    AssimpLoader(ModelManager *modelManager, MaterialManager *materialManager, TextureManager *textureManager, ShaderManager *shaderManager);
    AssimpLoader(const AssimpLoader& o);
    AssimpLoader& operator=(const AssimpLoader& o);
    ~AssimpLoader();

    void setAnimationTimeUnit(AnimationTimeUnit animationTimeUnit);
    AnimationTimeUnit getAnimationTimeUnit();

    Scene* load(std::string filename, bool yAxisFlipped = true);

  private:

    AssimpLoader(){}

    GroupNode* loadDefaultSceneGraph();
    void loadMaterialsFromAssimp(std::vector<ResourceHandle>& out_materials, const aiScene *scene);
    void loadMeshesFromAssimp(std::vector<ResourceHandle>& out_meshes, std::vector<ResourceHandle>& in_materials, const aiScene *scene, bool yAxisFlipped);
    ResourceHandle loadVertices(const aiMesh *mesh, ResourceHandle materialIndex, unsigned int meshIndex, bool yAxisFlipped);
    void loadAnimatedSkeleton(const aiScene *scene);
    GroupNode* loadSceneGraphFromAssimp(std::string filename, const aiNode *rootNode, std::vector<ResourceHandle> meshes);
    TreeNode* createSceneNodes(const aiNode *node, std::vector<ResourceHandle> meshes, GroupNode *parentNode, TreeNode *nextSibling);
    void attachBonesToSkinnedMesh();    

    AnimationTimeUnit m_animationTimeUnit;
    float m_animationTimeUnitConvert;

	  std::string m_modelPath;

    ModelManager *m_modelManager;
    MaterialManager *m_materialManager;
    TextureManager *m_textureManager;
    ShaderManager *m_shaderManager;

    std::map<std::string, std::vector<AnimationTrack>> m_animationTracks;//all the animation tracks per node
    std::vector<std::vector<Matrix<float, 4>>> m_inverseBindPoseTable;//all the boneMatrices per mesh
    std::vector<std::vector<std::string>> m_boneNameTable;

    std::map<AnimatedGeoNode*, std::vector<std::string>> m_skinnedMeshTable;//names of the skinned geo nodes
    std::map<std::string, AnimatedTransformNode*> m_boneTable;//names of the bones for skinning
  };
}
#endif