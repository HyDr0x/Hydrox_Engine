template<
	class C,
	typename R SIGNALS_COMMA_IF_ARGUMENTS 
	SIGNALS_TEMPLATE_PARAMS
> 
class SIGNALS_CLASSDELEGATE : public SIGNALS_DELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>
{
public:

	typedef R (C::*FuncPtr)(SIGNALS_TEMPLATE_ARGS);
	typedef SIGNALS_CLASSDELEGATE<C, R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS> Type;

	SIGNALS_CLASSDELEGATE(C *obj, FuncPtr fptr) : m_obj(obj),m_fptr(fptr)
	{}

	~SIGNALS_CLASSDELEGATE()
	{}

	R invoke(SIGNALS_PARAMS)
	{
		return (m_obj->*m_fptr)(SIGNALS_ARGS);
	}

	bool operator==(SIGNALS_DELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>* obj)
	{
		Type *tmpObj=dynamic_cast<Type*>(obj);
		if(tmpObj==NULL) return false;
		return m_fptr==tmpObj->m_fptr && m_obj==tmpObj->m_obj;
	}

private:

	FuncPtr m_fptr;
	C *m_obj;
};