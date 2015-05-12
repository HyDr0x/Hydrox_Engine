#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <map>
#include <vector>
#include <cassert>

#include "DataBase/DLLExport.h"

#include "DataBase/ManagedResource.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace db
  {
    class GRAPHICAPI Material : public ManagedResource
    {
    public:

      enum TextureType
      {
        DIFFUSETEX,
        NORMALTEX,
        DISPLACEMENTTEX,
        SPECULARTEX,
        TEXTURETYPENUM
      };

      struct MaterialData
      {
        MaterialData() : diffuseStrength(1.0f), specularStrength(1.0f), ambientStrength(1.0f), specularExponent(1.0f)
        {
        }

        MaterialData(float in_diffuseStrength, float in_specularStrength, float in_ambientStrength, float in_specularExponent) :
          diffuseStrength(in_diffuseStrength),
          specularStrength(in_specularStrength),
          ambientStrength(in_ambientStrength),
          specularExponent(in_specularExponent)
        {
        }

        float diffuseStrength;
        float specularStrength;//metall materials have their own color as specular reflection
        float ambientStrength;
        float specularExponent;
      };

      Material();
      Material(const Material& other);
      Material(const MaterialData& materialData, const std::vector< std::vector<util::ResourceHandle> >& textureHandles, std::vector< std::vector<uint64_t> > textureHashes, bool transparency, bool debug = false, util::vec4f uniColor = util::vec4f(0, 0, 0, 0));
      ~Material();

      Material& operator=(Material other);

      void free();

      void setTextureNumber(TextureType texType, unsigned int texNum);
      unsigned int getTextureNumber(TextureType texType) const;

      void setTextureHandle(TextureType texType, unsigned int slot, util::ResourceHandle textureHandle, uint64_t textureHash);
      util::ResourceHandle getTextureHandle(TextureType texType, unsigned int slot) const;
      std::vector< std::vector<util::ResourceHandle> > getTextureHandles() const;
      bool equalTextureHandles(const std::vector<std::vector<util::ResourceHandle>>& textureHandles) const;

      void setMaterialData(const MaterialData& material);
      const MaterialData& getMaterialData() const;

      bool getTransparency() const;

      bool getDebug() const;

      util::vec4f getUniColor() const;
      void setUniColor(util::vec4f uniColor);

    protected:

      virtual void updateHash();

    private:

      void swap(Material& other);

      std::vector< std::vector<uint64_t> > m_textureHashes;
      std::vector< std::vector<util::ResourceHandle> > m_textureHandles;
      MaterialData m_materialData;

      util::vec4f m_uniColor;
      bool m_debug;
      bool m_transparency;
    };
  }
}

#endif
