template<
	typename R SIGNALS_COMMA_IF_ARGUMENTS 
	SIGNALS_TEMPLATE_PARAMS
>
class SIGNALS_NONCLASSDELEGATE : public SIGNALS_DELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>
{
public:
	typedef R (*FuncPtr)(SIGNALS_TEMPLATE_ARGS);
	typedef SIGNALS_NONCLASSDELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS> Type;

	SIGNALS_NONCLASSDELEGATE(FuncPtr fptr) : m_fptr(fptr)
	{}

	~SIGNALS_NONCLASSDELEGATE()
	{}

	R invoke(SIGNALS_PARAMS)
	{
		return (*m_fptr)(SIGNALS_ARGS);
	}

	bool operator==(SIGNALS_DELEGATE<R SIGNALS_COMMA_IF_ARGUMENTS SIGNALS_TEMPLATE_ARGS>* obj)
	{
		Type *tmpObj=dynamic_cast<Type*>(obj);
		return obj!=NULL && reinterpret_cast<void*>(&m_fptr)==reinterpret_cast<void*>(&tmpObj->m_fptr);
	}

private:

	FuncPtr m_fptr;
};
