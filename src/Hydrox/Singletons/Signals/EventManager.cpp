#include "Hydrox/Singletons/Signals/EventManager.h"

namespace he
{
  void EventManager::createService()
  {
    m_instance = new EventManager();
  }

  EventManager::EventManager()
  {
  }

  EventManager::~EventManager()
  {
	  m_sigMap.clear();
  }
}
