#ifndef CACHEMANAGER_H_
#define CACHEMANAGER_H_

#include <map>
#include <vector>
#include <list>

#include "Hydrox/Utility/Observer/Observer.hpp"
#include "Hydrox/Singletons/Singleton.hpp"
#include "Hydrox/Utility/Miscellaneous/CachedResource.h"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Graphics/RenderShader.h"
#include "Hydrox/Graphics/ComputeShader.h"
#include "Hydrox/Graphics/Texture.h"
#include "Hydrox/Graphics/Billboard.h"
#include "Hydrox/Graphics/Sprite.h"

namespace he
{
  template<class CLASS> class CacheManager : public Singleton<CacheManager<CLASS>>, public Observer<ResourceHandle*>
  {
  public:

    CacheManager()
    {
    }

	  ~CacheManager()
    {
      for(unsigned int i = 0; i < m_objectsCache.size(); i++)
      {
        if(m_referenceCounter[i] > 0)
        {
          std::cout << "WARNING, Resource not being cleaned correctly!" << std::endl;
        }

        m_objectsCache[i].free();
      }
    }

    void initialize(std::string path)
    {
      m_path = path;
    }

    CLASS* getObject(ResourceHandle handle)
    {
      return dynamic_cast<CLASS*>(&m_objectsCache[handle.getID()]);
    }

    bool isAlreadyCached(CLASS& object)
    {
      return m_objectHash.count(object.getHash());
    }

    ResourceHandle addObject(CLASS& object)
    {
      uint64_t hash = object.getHash();

      if(!m_objectHash.count(hash))
      {
        if(m_list.empty())
        {
          unsigned int oldSize = m_objectsCache.size();
          unsigned int size = static_cast<unsigned int>((m_objectsCache.size() / m_BLOCKSIZE + 1) * m_BLOCKSIZE);
          m_objectsCache.resize(size);
          m_referenceCounter.resize(size, 0);
          for(unsigned int i = oldSize; i < size; i++)
          {
            m_list.push_back(i);
          }
        }

        unsigned int id = *m_list.begin();
        m_list.pop_front();

        ResourceHandle handle(id, &m_referenceCounter[id]);
        handle.add(this);

        m_objectsCache[handle.getID()] = object;
        m_objectHash[hash] = handle.getID();

        return handle;
      }
      else
      {
        unsigned int id = m_objectHash[hash];
        ResourceHandle handle(id, &m_referenceCounter[id]);
        handle.add(this);

        return handle;
      }
    }

    void updateObserver(ResourceHandle* param)
    {
      uint64_t hash = m_objectsCache[param->getID()].getHash();
      m_objectHash.erase(m_objectsCache[param->getID()].getHash());
      m_objectsCache[param->getID()].free();

      m_list.push_front(param->getID());
    }

	  std::string getPath() const
    {
	    return m_path;
    }

  private:

	  CacheManager(const CacheManager&){}

    static const unsigned int m_BLOCKSIZE = 64;

	  std::list<unsigned int> m_list;

	  std::vector<CLASS> m_objectsCache;
    std::vector<unsigned int> m_referenceCounter;
    std::map<uint64_t, unsigned int> m_objectHash;

    std::string m_path;
  };

  template class GRAPHICAPI CacheManager<Mesh>;
  template class GRAPHICAPI CacheManager<Material>;
  template class GRAPHICAPI CacheManager<RenderShader>;
  template class GRAPHICAPI CacheManager<ComputeShader>;
  template class GRAPHICAPI CacheManager<Texture>;
  template class GRAPHICAPI CacheManager<Billboard>;
  template class GRAPHICAPI CacheManager<Sprite>;

  typedef CacheManager<Mesh> ModelManager;
  typedef CacheManager<Material> MaterialManager;
  typedef CacheManager<RenderShader> RenderShaderManager;
  typedef CacheManager<ComputeShader> ComputeShaderManager;
  typedef CacheManager<Texture> TextureManager;
  typedef CacheManager<Billboard> BillboardManager;
  typedef CacheManager<Sprite> SpriteManager;
}

#endif
