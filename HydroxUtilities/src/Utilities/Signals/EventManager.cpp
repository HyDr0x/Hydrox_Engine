#include "Utilities/Signals/EventManager.h"

namespace he
{
	namespace util
	{
    EventManager::EventManager()
    {
    }

    EventManager::~EventManager()
    {
	    m_sigMap.clear();
    }
	}
}
