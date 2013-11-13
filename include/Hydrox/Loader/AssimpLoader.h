#ifndef ASSIMPLOADER_H_
#define ASSIMPLOADER_H_

#include <map>
#include <stack>
#include <string>
#include <assert.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Graphics/Mesh.h"

#include "Hydrox/Services/CacheManager.hpp"

class ILDevilLoader;

class GroupNode;
class TreeNode;

class Camera;

class Scene;

struct heAnimation
{
  heAnimation(std::string animationName, aiNodeAnim* nodeAnimation)
  {
    m_animationName = animationName;
    m_nodeName = nodeAnimation->mNodeName.C_Str();
    
    m_positions.resize(nodeAnimation->mNumPositionKeys);
    m_positionsTime.resize(nodeAnimation->mNumPositionKeys);

    m_rotations.resize(nodeAnimation->mNumRotationKeys);
    m_rotationsTime.resize(nodeAnimation->mNumRotationKeys);

    m_scales.resize(nodeAnimation->mNumScalingKeys);
    m_scalesTime.resize(nodeAnimation->mNumScalingKeys);

    for(unsigned int k = 0; k < nodeAnimation->mNumPositionKeys; k++)
    {
      aiVector3D pos = nodeAnimation->mPositionKeys[k].mValue;
       m_positions[k] = Vec<float, 3>(pos[0], pos[1], pos[2]);
       m_positionsTime[k] = nodeAnimation->mPositionKeys[k].mTime;
    }

    for(unsigned int k = 0; k < nodeAnimation->mNumRotationKeys; k++)
    {
      aiQuaternion rotation = nodeAnimation->mRotationKeys[k].mValue;
       m_rotations[k] = Quaternion<float>(rotation.w, rotation.x, rotation.y, rotation.z);
       m_rotationsTime[k] = nodeAnimation->mRotationKeys[k].mTime;
    }

    for(unsigned int k = 0; k < nodeAnimation->mNumScalingKeys; k++)
    {
      aiVector3D scale = nodeAnimation->mScalingKeys[k].mValue;
       m_scales[k] = Vec<float, 3>(scale[0], scale[1], scale[2]);
       m_scalesTime[k] = nodeAnimation->mScalingKeys[k].mTime;
    }
  }

  std::string m_animationName;
  std::string m_nodeName;

  std::vector<Vec<float, 3>>  m_positions;
  std::vector<float>  m_positionsTime;

  std::vector<Quaternion<float>>  m_rotations;
  std::vector<float>  m_rotationsTime;

  std::vector<Vec<float, 3>>  m_scales;
  std::vector<float>  m_scalesTime;
};

class GRAPHICAPI AssimpLoader
{
public:

  AssimpLoader(Camera *camera, ModelManager *modelManager, MaterialManager *materialManager, TextureManager *textureManager, ShaderManager *shaderManager);
  AssimpLoader(const AssimpLoader& o);
  AssimpLoader& operator=(const AssimpLoader& o);
  ~AssimpLoader();

  Scene* load(std::string filename, std::string materialFileName, bool yAxisFlipped = true);

private:

  AssimpLoader(){}

  void loadMeshesFromAssimp(std::vector<ResourceHandle>& meshes, std::string materialFileName, const aiScene *scene, bool yAxisFlipped);
  ResourceHandle loadVertices(const aiMesh *mesh, ResourceHandle materialIndex, bool yAxisFlipped);
  ResourceHandle loadMaterialsFromAssimp(std::string materialFileName, const aiScene *model, unsigned int id);
  GroupNode* loadSceneGraphFromAssimp(std::string filename, const aiNode *rootNode, std::vector<ResourceHandle> meshes);
  void loadAnimatedSkeleton(const aiScene *scene);
  void findAnimatedSkeleton();

  TreeNode* createSceneNodes(const aiNode *node, std::vector<ResourceHandle> meshes, GroupNode *parentNode, TreeNode *nextSibling);

	std::string m_modelPath;

  Camera *m_camera;
  ModelManager *m_modelManager;
  MaterialManager *m_materialManager;
  TextureManager *m_textureManager;
  ShaderManager *m_shaderManager;

  std::map<std::string, std::vector<heAnimation*>> m_animationTable;
  std::map<std::string, aiBone*> m_boneAnimationTable;
};

#endif