#include "Loader/RenderShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

namespace he
{
  namespace loader
  {
    RenderShaderLoader::RenderShaderLoader()
    {
    }

    RenderShaderLoader::~RenderShaderLoader()
    {
    }

    sh::RenderShader RenderShaderLoader::loadResource(
      sh::ShaderSlotFlags vertexDecaration,
      const std::string& filename,
      const std::vector<std::string>& shaderFilenames)
    {
      std::vector<std::string> shaderSources(5);

      for(unsigned int i = 0; i < shaderSources.size(); i++)
      {
        shaderSources[i] = loadShaderSource(shaderFilenames[i]);
      }

      if(shaderSources[0].empty())
      {
        std::clog << "ERROR, couldn't open file: " << filename << std::endl;
        assert(false);
      }

      return sh::RenderShader(vertexDecaration, filename, shaderFilenames, shaderSources);
    }

    void RenderShaderLoader::loadShadersInIndexfile(std::string path, std::string shaderIndexFilename)
    {
      if(shaderIndexFilename == std::string())
      {
        return;
      }

      util::SharedPointer<sh::ShaderContainer> container = m_singletonManager->getService<sh::ShaderContainer>();

      std::ifstream file(path + shaderIndexFilename);
      std::string line;
      std::string renderPassKeyword("RenderPass"), vertexDeclarationKeyword("VertexDeclaration");
      unsigned int renderPass = UINT32_MAX;
      unsigned int vertexDeclaration = UINT32_MAX;

      if(file.is_open())
      {
        while(!file.eof())
        {
          std::getline(file, line, '\n');
          //line += '\n';

          if(line.size() > 0 && line[0] == '#')
          {
            continue;
          }

          size_t pos = line.find(renderPassKeyword);
          if(pos != std::string::npos)
          {
            renderPass = atoi(line.substr(pos + renderPassKeyword.size() + 1).c_str());
            continue;
          }

          pos = line.find(vertexDeclarationKeyword);
          if(pos != std::string::npos && renderPass < UINT32_MAX)
          {
            vertexDeclaration = atoi(line.substr(pos + vertexDeclarationKeyword.size() + 1).c_str());

            std::string shaderName;
            std::getline(file, shaderName, '\n');

            std::vector<std::string> shaderFileNames(5);
            for(unsigned int i = 0; i < shaderFileNames.size(); i++)
            {
              std::getline(file, shaderFileNames[i], '\n');
              if(shaderFileNames[i] != "NULL")
              {
                shaderFileNames[i] = path + shaderFileNames[i];
              }
              else
              {
                shaderFileNames[i] = "";
              }
            }

            sh::RenderShaderHandle shaderHandle = container->addRenderShader(renderPass, loadResource(sh::ShaderSlotFlags(vertexDeclaration), shaderName, shaderFileNames));

            for(unsigned int i = 0; i < shaderFileNames.size(); i++)
            {
              if(!shaderFileNames[i].empty())
              {
                m_renderShaderMap[shaderFileNames[i]].push_back(shaderHandle);
              }
            }

            continue;
          }
        }

        file.close();
      }
      else
      {
        file.close();

        std::clog << "Warning, couldn't open render shader config file: " << path + shaderIndexFilename << std::endl;
      }
    }

    void RenderShaderLoader::checkIfShaderChanged()
    {
      if(m_shaderFileChecker.isFileChanged())
      {
        util::SharedPointer<sh::ShaderContainer> container = m_singletonManager->getService<sh::ShaderContainer>();

        std::vector<sh::RenderShaderHandle> editedShaderHandles = m_renderShaderMap[m_shaderFileChecker.getChangedFilepath()];

        for(unsigned int i = 0; i < editedShaderHandles.size(); i++)
        {
          const sh::RenderShader& shader = container->getRenderShader(editedShaderHandles[i]);
          container->replaceRenderShader(editedShaderHandles[i], loadResource(shader.getVertexDeclaration(), 
            shader.getShaderName(),
            shader.getShaderSourceNames()));
        }

        m_shaderFileChecker.resetFilecheckerStatus();
      }
    }
  }
}