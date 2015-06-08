#ifndef IMATERIALRENDERDATA_H_
#define IMATERIALRENDERDATA_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <XBar/IGeometryContainer.h>

#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Buffer/UBO.h"

#include "Renderer/Renderdata/IInstanceRenderData.h"

namespace he
{
  namespace renderer
  {
    struct MaterialIndexData
    {
      unsigned int bufferIndex;
      unsigned int instanceNumber;
    };

    class IMaterialRenderData
    {
    public:

      IMaterialRenderData(util::SingletonManager *singletonManager, std::vector<std::vector<util::ResourceHandle>> textureHandles) :
        m_textureHandles(textureHandles),
        m_materialCount(0),
        m_updateMaterialData(false)
      {
        m_materialManager = singletonManager->getService<db::MaterialManager>();
        m_options = singletonManager->getService<RenderOptions>();

        m_materialBuffer.createBuffer(sizeof(db::Material::MaterialData) * m_options->maxMaterialNumber, GL_STATIC_DRAW);
        m_materialIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * m_options->perInstanceBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      }

      virtual ~IMaterialRenderData() = 0 {}

      virtual bool canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual void insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;
      virtual void removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual void bindMaterialBuffer() const = 0;
      virtual void unbindMaterialBuffer() const = 0;

      virtual void updateBuffer(IInstanceRenderData& renderData) = 0;

      virtual const std::vector<std::vector<util::ResourceHandle>>& getTextureHandles() const = 0;

    protected:

      util::SharedPointer<db::MaterialManager>m_materialManager;
      util::SharedPointer<RenderOptions> m_options;

      std::vector<std::vector<util::ResourceHandle>> m_textureHandles;

      UBO m_materialBuffer;
      GPUImmutableBuffer m_materialIndexBuffer;

      bool m_updateMaterialData;

      unsigned int m_materialCount;

      std::map<util::ResourceHandle, MaterialIndexData, util::ResourceHandle::Less> m_materialHandles;
      std::map<util::ResourceHandle, uint64_t, util::ResourceHandle::Less> m_materialHashes;//original materialHandle with the actual hash value and data and the old hash value which stands for the old data uploaded to the gpu
    };
  }
}

#endif
