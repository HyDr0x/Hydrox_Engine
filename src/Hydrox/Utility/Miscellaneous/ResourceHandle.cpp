#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

namespace he
{
  ResourceHandle::ResourceHandle()
  {
    m_referenceCounter = nullptr;
    m_id = ~0;
  }

  ResourceHandle::ResourceHandle(unsigned int id)
  {
    m_referenceCounter = new unsigned int(1);
    m_id = id;
  }

  ResourceHandle::ResourceHandle(const ResourceHandle& o) : m_referenceCounter(o.m_referenceCounter), m_id(o.m_id)
  {
    if(m_referenceCounter != nullptr)
    {
      (*m_referenceCounter)++;
    }

    m_observer = o.m_observer;
  }

  ResourceHandle& ResourceHandle::operator=(const ResourceHandle& o)
  {
    if(m_id == o.m_id)
    {
      return *this;
    }

    if(m_referenceCounter != nullptr && --(*m_referenceCounter) == 0)
    {
      notify(this);
      delete m_referenceCounter;
    }

    m_referenceCounter = o.m_referenceCounter;
    m_id = o.m_id;
    (*m_referenceCounter)++;

    m_observer = o.m_observer;

    return *this;
  }

  ResourceHandle::~ResourceHandle()
  {
    if(m_referenceCounter != nullptr && --(*m_referenceCounter) == 0)
    {
      notify(this);
      delete m_referenceCounter;
      m_referenceCounter = nullptr;
    }
  }

  unsigned int ResourceHandle::getID()
  {
    return m_id;
  }
}