#include "Renderer/Resources/ManagedResource.h"

namespace he
{
	namespace renderer
	{
	

    ManagedResource::ManagedResource()
    {
    }

    ManagedResource::ManagedResource(const ManagedResource& o)
    {
      m_hash = o.m_hash;
    }

    ManagedResource& ManagedResource::operator=(const ManagedResource& o)
    {
      m_hash = o.m_hash;

      return *this;
    }

    ManagedResource::~ManagedResource()
    {
    }

    const uint64_t ManagedResource::getHash() const
    {
      return m_hash;
    }

	}
}