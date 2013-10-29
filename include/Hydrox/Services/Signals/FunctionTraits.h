#ifndef FUNCTIONTRAITS_H_
#define FUNCTIONTRAITS_H_

template<typename Signature> struct FunctionTrait;//dummy struct, shouldn't be instanciated
//{
//	const static unsigned char arity=CHAR_MAX;//dummy arity
//	typedef Signature returnType;//dummy typedef
//};

////////////////////////////NON MEMBER FUNCTION POINTER//////////////////////////////
template<typename R> struct FunctionTrait<R(*)(void)>
{
	const static unsigned char arity=0;
	typedef R returnType;
};

template<typename R, typename T1> struct FunctionTrait<R(*)(T1)>
{
	const static unsigned char arity=1;
	typedef R returnType;
	typedef T1 arg1Type;
};

template<typename R, typename T1, typename T2> struct FunctionTrait<R(*)(T1,T2)>
{
	const static unsigned char arity=2;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
};

template<typename R, typename T1, typename T2, typename T3> struct FunctionTrait<R(*)(T1,T2,T3)>
{
	const static unsigned char arity=3;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
};

template<typename R, typename T1, typename T2, typename T3, typename T4> struct FunctionTrait<R(*)(T1,T2,T3,T4)>
{
	const static unsigned char arity=4;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
};

template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5> struct FunctionTrait<R(*)(T1,T2,T3,T4,T5)>
{
	const static unsigned char arity=5;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
};

template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> struct FunctionTrait<R(*)(T1,T2,T3,T4,T5,T6)>
{
	const static unsigned char arity=6;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
};

template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> struct FunctionTrait<R(*)(T1,T2,T3,T4,T5,T6,T7)>
{
	const static unsigned char arity=7;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
	typedef T7 arg7Type;
};

template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> struct FunctionTrait<R(*)(T1,T2,T3,T4,T5,T6,T7,T8)>
{
	const static unsigned char arity=8;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
	typedef T7 arg7Type;
	typedef T8 arg8Type;
};

template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9> struct FunctionTrait<R(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>
{
	const static unsigned char arity=9;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
	typedef T7 arg7Type;
	typedef T8 arg8Type;
	typedef T9 arg9Type;
};

template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10> struct FunctionTrait<R(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>
{
	const static unsigned char arity=10;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
	typedef T7 arg7Type;
	typedef T8 arg8Type;
	typedef T9 arg9Type;
	typedef T10 arg10Type;
};

///////////////////////////////MEMBER FUNCTION POINTER///////////////////////////////
template<class C, typename R> struct FunctionTrait<R(C::*)(void)>
{
	const static unsigned char arity=0;
	typedef R returnType;
};

template<class C, typename R, typename T1> struct FunctionTrait<R(C::*)(T1)>
{
	const static unsigned char arity=1;
	typedef R returnType;
	typedef T1 arg1Type;
};

template<class C, typename R, typename T1, typename T2> struct FunctionTrait<R(C::*)(T1,T2)>
{
	const static unsigned char arity=2;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
};

template<class C, typename R, typename T1, typename T2, typename T3> struct FunctionTrait<R(C::*)(T1,T2,T3)>
{
	const static unsigned char arity=3;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
};

template<class C, typename R, typename T1, typename T2, typename T3, typename T4> struct FunctionTrait<R(C::*)(T1,T2,T3,T4)>
{
	const static unsigned char arity=4;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
};

template<class C, typename R, typename T1, typename T2, typename T3, typename T4, typename T5> struct FunctionTrait<R(C::*)(T1,T2,T3,T4,T5)>
{
	const static unsigned char arity=5;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
};

template<class C, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> struct FunctionTrait<R(C::*)(T1,T2,T3,T4,T5,T6)>
{
	const static unsigned char arity=6;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
};

template<class C, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> struct FunctionTrait<R(C::*)(T1,T2,T3,T4,T5,T6,T7)>
{
	const static unsigned char arity=7;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
	typedef T7 arg7Type;
};

template<class C, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> struct FunctionTrait<R(C::*)(T1,T2,T3,T4,T5,T6,T7,T8)>
{
	const static unsigned char arity=8;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
	typedef T7 arg7Type;
	typedef T8 arg8Type;
};

template<class C, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9> struct FunctionTrait<R(C::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>
{
	const static unsigned char arity=9;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
	typedef T7 arg7Type;
	typedef T8 arg8Type;
	typedef T9 arg9Type;
};

template<class C, typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10> struct FunctionTrait<R(C::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>
{
	const static unsigned char arity=10;
	typedef R returnType;
	typedef T1 arg1Type;
	typedef T2 arg2Type;
	typedef T3 arg3Type;
	typedef T4 arg4Type;
	typedef T5 arg5Type;
	typedef T6 arg6Type;
	typedef T7 arg7Type;
	typedef T8 arg8Type;
	typedef T9 arg9Type;
	typedef T10 arg10Type;
};
#endif