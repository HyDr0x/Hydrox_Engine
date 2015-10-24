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
      const std::string& filename,
      const std::vector<std::string>& shaderFilenames)
    {
      std::vector<std::string> shaderSources(5);

      for(unsigned int i = 0; i < shaderSources.size(); i++)
      {
        shaderSources[i] = loadShaderSource(shaderFilenames[i]);
      }

      return sh::RenderShader(readVertexDeclaration(shaderSources[0]), filename, shaderFilenames, shaderSources);
    }

    void RenderShaderLoader::loadShadersInIndexfile(std::string path, std::string shaderIndexFilename)
    {
      if(shaderIndexFilename == std::string())
      {
        return;
      }

      std::ifstream file(path + shaderIndexFilename);
      std::string line;
      std::string renderPassKeyword("Renderpass"), newShaderKeyword("Shader");
      unsigned int renderPass = UINT32_MAX;

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

          pos = line.find(newShaderKeyword);
          if(pos != std::string::npos && renderPass < UINT32_MAX)
          {
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

            sh::RenderShaderHandle shaderHandle = m_container->addRenderShader(renderPass, loadResource(shaderName, shaderFileNames));

            for(unsigned int i = 0; i < shaderFileNames.size(); i++)
            {
              if(!shaderFileNames[i].empty())
              {
                registerShaderSourceFiles(shaderFileNames[i], shaderHandle);
                //m_renderShaderMap[shaderFileNames[i]].push_back(shaderHandle);
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
        std::vector<sh::RenderShaderHandle> editedShaderHandles = m_renderShaderMap[m_shaderFileChecker.getChangedFilepath()];

        for(unsigned int i = 0; i < editedShaderHandles.size(); i++)
        {
          const sh::RenderShader& shader = m_container->getRenderShader(editedShaderHandles[i]);
          m_container->replaceRenderShader(editedShaderHandles[i], loadResource(shader.getShaderName(), shader.getShaderSourceNames()));
        }

        m_shaderFileChecker.resetFilecheckerStatus();
      }
    }

    sh::ShaderSlotFlags RenderShaderLoader::readVertexDeclaration(const std::string& vertexShaderSource)
    {
      std::string searchString("layout(location = ");
      size_t stringPosNumberBegin = 0, stringPosNumberEnd = 0;
      sh::ShaderSlotFlags vertexDeclarationFlags;
      
      do
      {
        stringPosNumberBegin = vertexShaderSource.find(searchString, stringPosNumberEnd);

        if(stringPosNumberBegin != std::string::npos)
        {
          stringPosNumberBegin += searchString.size();
          stringPosNumberEnd = vertexShaderSource.find(')', stringPosNumberBegin);

          size_t stringPosWhitespace = vertexShaderSource.find_first_not_of(' ', stringPosNumberEnd + 1);//look if it is a vertex declaration location and not a uniform or anything else

          if(vertexShaderSource.substr(stringPosWhitespace, 2) == "in")
          {
            unsigned long vertexDeclaration = std::strtoul(vertexShaderSource.substr(stringPosNumberBegin, stringPosNumberEnd - stringPosNumberBegin).c_str(), nullptr, 0);
            vertexDeclarationFlags |= sh::ShaderSlotFlags::convertToFlag(vertexDeclaration);
          }
        }
        
      } while(stringPosNumberBegin != std::string::npos);

      return vertexDeclarationFlags;
    }

    void RenderShaderLoader::registerShaderSourceFiles(const std::string& shaderFilename, sh::RenderShaderHandle shaderHandle)
    {
      bool alreadyIn = false;
      for(unsigned int i = 0; i < m_renderShaderMap[shaderFilename].size(); i++)
      {
        if(m_renderShaderMap[shaderFilename][i] == shaderHandle)
        {
          alreadyIn = true;
          break;
        }
      }

      if(!alreadyIn)//only insert it if it's not already in there
      {
        m_renderShaderMap[shaderFilename].push_back(shaderHandle);
      }

      std::string shaderSource = loadFile(shaderFilename);

      size_t stringPos = 0;
      std::string includeString = "#include";

      do
      {
        stringPos = shaderSource.find(includeString, stringPos);

        if(stringPos != std::string::npos)
        {
          size_t filenameStart = shaderSource.find('"', stringPos) + 1;
          size_t filenameEnd = shaderSource.find('"', filenameStart + 1);
          std::string filename = shaderSource.substr(filenameStart, filenameEnd - filenameStart);

          registerShaderSourceFiles(filename, shaderHandle);

          stringPos++;//to not find the same include again
        }

      } while(stringPos != std::string::npos);
    }
  }
}