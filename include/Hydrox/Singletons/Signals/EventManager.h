#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

#include <map>

#include "Hydrox/Singletons/Singleton.hpp"
#include "Hydrox/Singletons/Signals/Signal.h"

//const member functions or functions with variadic arguments are not supported!

namespace he
{
  class EventManager : public Singleton<EventManager>
  {
  public:

    enum Events
	  {
		  OnDefault,
		  OnCollision,
		  OnMouseClick,
		  OnKeyHit
	  };

    static void EventManager::createService()
    {
      m_instance = new EventManager();
    }

	  ~EventManager();

	  template<typename Signature> void addNewEvent(int _Event)
	  {
		  m_sigMap[_Event]=dynamic_cast<signal_save*>(new Signal<Signature>);
	  }

	  void deleteEvent(int _Event)
	  {
		  delete m_sigMap[_Event];
		  m_sigMap.erase(_Event);
	  }

	  template<typename Signature> void addFuncToEvent(Signature fptr, int _Event)
	  {
		  dynamic_cast<Signal<Signature>*>(m_sigMap[_Event])->connect(fptr);
	  }

	  template<class C, typename SignalSignature, typename FuncSignature> void addFuncToEvent(C *obj, FuncSignature fptr, int _Event)
	  {
		  dynamic_cast<Signal<SignalSignature>*>(m_sigMap[_Event])->connect<C>(obj,fptr);
	  }

	  template<typename Signature> void deleteFuncFromEvent(Signature fptr, int _Event)
	  {
		  dynamic_cast<Signal<Signature>*>(m_sigMap[_Event])->disconnect(fptr);
	  }

	  template<class C, typename SignalSignature, typename FuncSignature> void deleteFuncFromEvent(C *obj, FuncSignature fptr, int _Event)
	  {
		  dynamic_cast<Signal<SignalSignature>*>(m_sigMap[_Event])->disconnect(obj,fptr);
	  }

	  template<typename Signature> Signal<Signature>* raiseSignal(int _Event)
	  {
		  return dynamic_cast<Signal<Signature>*>(m_sigMap[_Event]);
	  }

	  template<typename Signature> unsigned int getSlotNumber(int _Event)
	  {
		  return dynamic_cast<Signal<Signature>*>(m_sigMap[_Event])->getSlotNumber();
	  }

  private:

    EventManager();

	  std::map<int,signal_save*> m_sigMap;
  };
}

#endif