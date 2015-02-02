#include "Loader/RenderShaderLoader.h"

#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/RenderShader.h>
#include <DataBase/ShaderContainer.h>

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
      util::Flags<db::VertexDeclarationFlags> vertexDecaration,
      std::string filename,
      std::string vertexFilename,
      std::string fragmentFilename,
      std::string geometryFilename,
      std::string tessControlFilename,
      std::string tessEvalFilename)
    {
      std::string vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource;

      vertexShaderSource = loadShaderSource(vertexFilename);
      fragmentShaderSource = loadShaderSource(fragmentFilename);
      geometryShaderSource = loadShaderSource(geometryFilename);
      tesselationCTRLShaderSource = loadShaderSource(tessControlFilename);
      tesselationEVALShaderSource = loadShaderSource(tessEvalFilename);

      util::ResourceHandle shaderHandle;

      if(vertexShaderSource.empty())
      {
        std::cerr << "ERROR, couldn't open file: " << vertexFilename << std::endl;
        assert(false);
      }
      else
      {
        shaderHandle = m_singletonManager->getService<db::RenderShaderManager>()->addObject(db::RenderShader(vertexDecaration, filename, vertexShaderSource, fragmentShaderSource, geometryShaderSource, tesselationCTRLShaderSource, tesselationEVALShaderSource));
      }

      return shaderHandle;
    }

    void RenderShaderLoader::loadIndexfile(std::string path, std::string shaderIndexFilename)
    {
      if(shaderIndexFilename == std::string())
      {
        return;
      }

      util::SharedPointer<db::ShaderContainer> container = m_singletonManager->getService<db::ShaderContainer>();

      std::ifstream file(path + shaderIndexFilename);
      std::string line;
      unsigned int renderPass = 0;
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

          size_t pos = line.find("RenderPass");
          if(pos != std::string::npos)
          {
            renderPass++;
            continue;
          }

          pos = line.find("VertexDeclaration");
          if(pos != std::string::npos && renderPass - 1 < UINT32_MAX)
          {
            vertexDeclaration = atoi(line.substr(pos + 18).c_str());

            std::vector<std::string> shaderFileNames(6);
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

            container->m_renderPassShader[renderPass - 1].push_back(loadResource(util::Flags<db::VertexDeclarationFlags>(vertexDeclaration), shaderFileNames[0], shaderFileNames[1], shaderFileNames[2], shaderFileNames[3], shaderFileNames[4], shaderFileNames[5]));

            continue;
          }
        }

        file.close();
      }
      else
      {
        file.close();

        //std::cerr << "Warning, couldn't open shader source file: " << filename << std::endl;
      }
    }
  }
}