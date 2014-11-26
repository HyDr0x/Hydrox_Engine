#include "Loader/RenderShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/RenderShader.h>

namespace he
{
  namespace loader
  {
    RenderShaderLoader::RenderShaderLoader(util::SingletonManager *singletonManager) : ShaderLoader(singletonManager)
    {
    }

    RenderShaderLoader::~RenderShaderLoader()
    {
    }

    util::ResourceHandle RenderShaderLoader::loadResource(
      std::string filename,
      std::string vertexFilename,
      std::string fragmentFilename,
      std::string geometryFilename,
      std::string tessControlFilename,
      std::string tessEvalFilename)
    {
      std::string vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource;

      vertexShaderSource = loadShaderSource(vertexFilename + ".vert");
      fragmentShaderSource = loadShaderSource(fragmentFilename + ".frag");
      geometryShaderSource = loadShaderSource(geometryFilename + ".geom");
      tesselationCTRLShaderSource = loadShaderSource(tessControlFilename + ".control");
      tesselationEVALShaderSource = loadShaderSource(tessEvalFilename + ".eval");

      util::ResourceHandle shaderHandle;

      if(vertexShaderSource.empty())
      {
        std::cerr << "ERROR, couldn't open file: " << vertexFilename << std::endl;

        shaderHandle = getDefaultResource();
      }
      else
      {
        shaderHandle = m_singletonManager->getService<db::RenderShaderManager>()->addObject(db::RenderShader(filename, vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource));
      }

      return shaderHandle;
    }

    util::ResourceHandle RenderShaderLoader::getDefaultResource() const
    {
      std::string vertexSource = "#version 440 core\n\
                                  #extension ARB_shader_draw_parameters : enable\n\
                                  \n\
                                  layout(std140, binding = 0) uniform cameraParameters\n\
                                  {\n\
                                    mat4 viewMatrix;\n\
                                    mat4 projectionMatrix;\n\
                                    mat4 viewProjectionMatrix;\n\
                                    vec4 eyePos;\n\
                                  };\n\
                                  \n\
                                  layout(std430, binding = 0) buffer transformMatrixBuffer\n\
                                  {\n\
                                    mat4 trfMatrix[];\n\
                                  };\n\
                                  \n\
                                  layout(location = 0) in vec3 in_Pos;\n\
                                  \n\
                                  void main()\n\
                                  {\n\
                                    gl_Position = viewProjectionMatrix * trfMatrix[gl_InstanceID + gl_BaseInstanceARB] * vec4(in_Pos, 1);\n\
                                  }";

      std::string fragmentSource = "#version 440 core\n\
                                    layout(early_fragment_tests) in;\n\
                                    \n\
                                    out vec4 color;\n\
                                    \n\
                                    void main()\n\
                                    {\n\
                                      color = vec4(0.5f, 0.5f, 0.5f, 1);\n\
                                    }";

      return m_singletonManager->getService<db::RenderShaderManager>()->addObject(db::RenderShader(std::string("defaultRenderShader"), vertexSource, fragmentSource));
    }

    util::ResourceHandle RenderShaderLoader::getDefaultShadowResource() const
    {
      std::string vertexSource = "#version 440 core\n\
                                 \n\
                                  #extension ARB_shader_draw_parameters : enable\n\
                                  \n\
                                  struct LightData\n\
                                  {\n\
                                    vec4 position;//w component = spotLightExponent\n\
                                    vec4 direction;//w component = spotLightCutoff | cos(lightAngle) so it should be between 0.0f (no cutoff) and 1.0f (full cutoff)\n\
                                                                        \n\
                                    vec4 color;//w component = unused\n\
                                                                        \n\
                                    float luminousFlux;\n\
                                                                        \n\
                                    float constAttenuation;\n\
                                    float linearAttenuation;\n\
                                    float quadricAttenuation;\n\
                                  };\n\
                                                                    \n\
                                  struct ShadowLightData\n\
                                  {\n\
                                    mat4 lightViewProj;\n\
                                                                        \n\
                                    vec4 projectionParameter;//x = near, y = nearWidth\n\
                                                                        \n\
                                    LightData light;\n\
                                  };\n\
                                  layout(std430, binding = 0) buffer transformMatrixBuffer\n\
                                  {\n\
                                    mat4 trfMatrix[];\n\
                                  };\n\
                                  \n\
                                  layout(std430, binding = 4) buffer relfectiveShadowLightBuffer\n\
                                  {\n\
                                    ShadowLightData reflectiveShadowLight[];\n\
                                  };\n\
                                  \n\
                                  layout(location = 2) uniform int lightIndex;\n\
                                  \n\
                                  layout(location = 0) in vec3 in_Pos;\n\
                                  layout(location = 5) in vec3 in_normal;\n\
                                  layout(location = 10) in vec4 in_color;\n\
                                  \n\
                                  out vec4 vsout_pos;\n\
                                  out vec3 vsout_normal;\n\
                                  flat out uint vsout_instanceIndex;\n\
                                  out vec4 vsout_color; \n\
                                  \n\
                                  void main()\n\
                                  {\n\
                                    vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);\n\
                                    vsout_color = in_color;\n\
                                    vsout_pos = trfMatrix[vsout_instanceIndex] * vec4(in_Pos, 1.0f);\n\
                                    vsout_normal = normalize(mat3(trfMatrix[vsout_instanceIndex]) * in_normal);\n\
                                    gl_Position = reflectiveShadowLight[lightIndex].lightViewProj * vsout_pos;\n\
                                  }\n\
                                ";

      std::string fragmentSource =  "#version 440 core\n\
                                                                        \n\
                                    layout(early_fragment_tests) in;\n\
                                                                        \n\
                                    struct MaterialData\n\
                                    {\n\
                                      float diffuseStrength;\n\
                                      float specularStrength;//metall materials have their own color as specular reflection\n\
                                      float ambientStrength;\n\
                                      float specularExponent;\n\
                                    };\n\
                                                                        \n\
                                    layout(std140, binding = 1) uniform materialBuffer\n\
                                    {\n\
                                      MaterialData material[512];\n\
                                    };\n\
                                                                        \n\
                                    layout(std430, binding = 1) buffer materialIndexBuffer\n\
                                    {\n\
                                      uint materialIndex[];\n\
                                    };\n\
                                                                        \n\
                                    struct LightData\n\
                                    {\n\
                                      vec4 position;//w component = spotLightExponent\n\
                                      vec4 direction;//w component = spotLightCutoff | cos(lightAngle) so it should be between 0.0f (no cutoff) and 1.0f (full cutoff)\n\
                                                                          \n\
                                      vec4 color;//w component = unused\n\
                                                                          \n\
                                      float luminousFlux;\n\
                                                                          \n\
                                      float constAttenuation;\n\
                                      float linearAttenuation;\n\
                                      float quadricAttenuation;\n\
                                    };\n\
                                                                      \n\
                                    struct ShadowLightData\n\
                                    {\n\
                                      mat4 lightViewProj;\n\
                                                                          \n\
                                      vec4 projectionParameter;//x = near, y = nearWidth\n\
                                                                          \n\
                                      LightData light;\n\
                                    };\n\
                                    \n\
                                    layout(location = 0) out vec4 fsout_pos3D; \n\
                                    layout(location = 1) out vec4 fsout_normal; \n\
                                    layout(location = 2) out vec4 fsout_luminousFlux; \n\
                                    \n\
                                    layout(location = 0) uniform sampler2D colorSampler; \n\
                                    \n\
                                    layout(std430, binding = 4) buffer reflectiveShadowLightBuffer\n\
                                    {\n\
                                    ShadowLightData reflectiveShadowLight[]; \n\
                                    }; \n\
                                                                        \n\
                                    layout(location = 2) uniform int lightIndex;\n\
                                    layout(location = 3) uniform uint shadowMapWidth;\n\
                                                                        \n\
                                    in vec4 vsout_pos;\n\
                                    in vec3 vsout_normal;\n\
                                    flat in uint vsout_instanceIndex;\n\
                                    in vec4 vsout_color;\n\
                                                                        \n\
                                    void main()\n\
                                    {\n\
                                      float zNear = reflectiveShadowLight[lightIndex].projectionParameter.x;\n\
                                      float nearArea = reflectiveShadowLight[lightIndex].projectionParameter.y;\n\
                                      float area = gl_FragCoord.z * gl_FragCoord.z * nearArea * nearArea / (zNear * zNear * shadowMapWidth * shadowMapWidth);\n\
                                                                            \n\
                                      fsout_pos3D = vsout_pos;\n\
                                      fsout_normal = vec4(vsout_normal * 0.5f + 0.5f, area);\n\
                                                                            \n\
                                      vec3 lightDir = reflectiveShadowLight[lightIndex].light.position.xyz - vsout_pos.xyz;\n\
                                      float distance = dot(lightDir, lightDir);\n\
                                      lightDir = normalize(lightDir);\n\
                                      \n\
                                      fsout_luminousFlux = max(dot(lightDir, vsout_normal), 0.0f) * reflectiveShadowLight[lightIndex].light.luminousFlux * material[materialIndex[vsout_instanceIndex]].diffuseStrength * vsout_color / distance;\n\
                                    };\n\
                                    ";

      return m_singletonManager->getService<db::RenderShaderManager>()->addObject(db::RenderShader(std::string("defaultReflectiveShadowShader"), vertexSource, fragmentSource));
    }
  }
}