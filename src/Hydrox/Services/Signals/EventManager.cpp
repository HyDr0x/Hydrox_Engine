#include "Hydrox/Services/Signals/EventManager.h"

namespace he
{
  EventManager::EventManager()
  {
  }

  EventManager::~EventManager()
  {
	  m_sigMap.clear();
  }
}
