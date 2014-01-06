#include <list>

#if SIGNALS_NUM_ARGS == 0
#	define SIGNALS_COMMA_IF_ARGUMENTS
#else
#	define SIGNALS_COMMA_IF_ARGUMENTS ,
#endif

#define SIGNALS_SIGNAL SIGNALS_JOIN(Signal,SIGNALS_NUM_ARGS)

#define SIGNALS_DELEGATE SIGNALS_JOIN(Delegate,SIGNALS_NUM_ARGS)
#define SIGNALS_CLASSDELEGATE SIGNALS_JOIN(ClassDelegate,SIGNALS_NUM_ARGS)
#define SIGNALS_NONCLASSDELEGATE SIGNALS_JOIN(NonClassDelegate,SIGNALS_NUM_ARGS)

#include "Hydrox/Singletons/Signals/Delegate.h"
#include "Hydrox/Singletons/Signals/ClassDelegate.h"
#include "Hydrox/Singletons/Signals/NonClassDelegate.h"

namespace he
{
  template<
	  typename R SIGNALS_COMMA_IF_ARGUMENTS 
	  SIGNALS_TEMPLATE_PARAMS
  > 
  class SIGNALS_SIGNAL
  {
  public:

	  SIGNALS_SIGNAL()
	  {}

	  ~SIGNALS_SIGNAL()
	  {
		  for(m_dit=m_DList.begin();m_dit!=m_DList.end();m_dit++)
			  delete (*m_dit);

		  m_DList.clear();
	  }


	  void execute(R* cargo SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_PARAMS)
	  {
		  unsigned int i=0;
		  for(m_dit=m_DList.begin();m_dit!=m_DList.end();m_dit++,i++)
			  cargo[i]=(*m_dit)->invoke(SIGNALS_ARGS);
	  }

  private:

	  typedef SIGNALS_DELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS> Delegate;

	  void connect(R(*fptr)(SIGNALS_TEMPLATE_ARGS))
	  {
		  m_DList.push_back(dynamic_cast<Delegate*>(new SIGNALS_NONCLASSDELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>(fptr)));
	  }

	  template<class C> void connect(C& obj, R(C::*fptr)(SIGNALS_TEMPLATE_ARGS))
	  {
		  m_DList.push_back(dynamic_cast<Delegate*>(new SIGNALS_CLASSDELEGATE<C, R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>(obj,fptr)));
	  }

	  void disconnect(R(*fptr)(SIGNALS_TEMPLATE_ARGS))
	  {
		  Delegate* tmpObj=dynamic_cast<Delegate*>(new SIGNALS_NONCLASSDELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>(fptr));
		  for(m_dit=m_DList.begin();m_dit!=m_DList.end();m_dit++)
			  if((*m_dit)==tmpObj)
			  {
				  m_dit=m_DList.erase(m_dit);
				  m_dit--;
			  }
	  }

	  template<class C> void disconnect(C *obj, R(C::*fptr)(SIGNALS_TEMPLATE_ARGS))
	  {
		  Delegate* tmpObj=dynamic_cast<Delegate*>(new SIGNALS_CLASSDELEGATE<C, R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>(obj,fptr));
		  for(m_dit=m_DList.begin();m_dit!=m_DList.end();m_dit++)
			  if((*m_dit)->operator==(tmpObj))
			  {
				  m_dit=m_DList.erase(m_dit);
				  m_dit--;
			  }
	  }

	  unsigned int getSlotNumber() const
	  {
		  return m_DList.size();
	  }

	  friend class EventManager;

	  typename std::list<Delegate*> m_DList;
	  typename std::list<Delegate*>::iterator m_dit;
  };


  ////////////////////////////PARTIAL SPEZIALIZATION FOR VOID RETURN TYPE////////////////////////////////
  #if SIGNALS_NUM_ARGS == 0
  #	define SIGNALS_TYPENAME
  #else
  #	define SIGNALS_TYPENAME typename
  #endif

  template< 
	  SIGNALS_TEMPLATE_PARAMS
  > 
  class SIGNALS_SIGNAL<
	  void SIGNALS_COMMA_IF_ARGUMENTS 
	  SIGNALS_TEMPLATE_ARGS
  >
  {
  public:

	  SIGNALS_SIGNAL()
	  {}

	  ~SIGNALS_SIGNAL()
	  {
		  for(m_dit=m_DList.begin();m_dit!=m_DList.end();m_dit++)
			  delete (*m_dit);

		  m_DList.clear();
	  }


	  void execute(SIGNALS_PARAMS)
	  {
		  for(m_dit=m_DList.begin();m_dit!=m_DList.end();m_dit++)
			  (*m_dit)->invoke(SIGNALS_ARGS);
	  }

  private:

	  typedef SIGNALS_DELEGATE<void SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS> Delegate;

	  void connect(void(*fptr)(SIGNALS_TEMPLATE_ARGS))
	  {
		  m_DList.push_back(dynamic_cast<Delegate*>(new SIGNALS_NONCLASSDELEGATE<void SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>(fptr)));
	  }

	  template<class C> void connect(C *obj, void(C::*fptr)(SIGNALS_TEMPLATE_ARGS))
	  {
		  m_DList.push_back(dynamic_cast<Delegate*>(new SIGNALS_CLASSDELEGATE<C, void SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>(obj,fptr)));
	  }

	  void disconnect(void(*fptr)(SIGNALS_TEMPLATE_ARGS))
	  {
		  Delegate* tmpObj=dynamic_cast<Delegate*>(new SIGNALS_NONCLASSDELEGATE<void SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>(fptr));
		  for(m_dit=m_DList.begin();m_dit!=m_DList.end();m_dit++)
			  if((*m_dit)==tmpObj)
			  {
				  m_dit=m_DList.erase(m_dit);
				  m_dit--;
			  }
	  }

	  template<class C> void disconnect(C *obj, void(C::*fptr)(SIGNALS_TEMPLATE_ARGS))
	  {
		  Delegate* tmpObj=dynamic_cast<Delegate*>(new SIGNALS_CLASSDELEGATE<C, void SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>(obj,fptr));
		  for(m_dit=m_DList.begin();m_dit!=m_DList.end();m_dit++)
			  if((*m_dit)->operator==(tmpObj))
			  {
				  m_dit=m_DList.erase(m_dit);
				  m_dit--;
			  }
	  }

	  friend class EventManager;

	  SIGNALS_TYPENAME std::list<Delegate*> m_DList;
	  SIGNALS_TYPENAME std::list<Delegate*>::iterator m_dit;
  };
}

#undef SIGNALS_COMMA_IF_ARGUMENTS
#undef SIGNALS_TYPENAME
