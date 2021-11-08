#pragma once
#include <string>
#include <map>

#include "serialize.h"
#include "func.h"

//template <typename T>
//class ParameterBase {
//public:
//	virtual T getpmt() = 0;
//	virtual T getpmt(const string& buf) = 0;
//};
//
//template <typename T>
//class Parameter :ParameterBase<T> {
//public:
//	Parameter(T& t)
//	{
//		pmt = move(t);
//	}
//	virtual T getpmt()
//	{
//		return move(pmt);
//	}
//	virtual T getpmt(const string& buf)
//	{
//		T ret = deserilize<T>(buf);
//		return move(ret);
//	}
//private:
//	T pmt;
//};
//该头文件定义总的调用函数，
//通过函数的唯一编码和入参调用相关函数，将返回值序列化为字符串后返回
std::string funcCall(int funcNum, string& buf)
{
	try {
		switch (funcNum)
		{		
			case 0:
			{
				A para = deserilize<A>(buf);
				B res = m_sum(para);
				string ret = serialize<B>(res);
				return move(ret);
			}
			case 1:
			{
				A para = deserilize<A>(buf);
				B res = m_sub(para);
				string ret = serialize<B>(res);
				return move(ret);
			}
			case 2:
			{
				A para = deserilize<A>(buf);
				B res = m_multi(para);
				string ret = serialize<B>(res);
				return move(ret);
			}
			case 3:
			{
				A para = deserilize<A>(buf);
				B res = m_dvd(para);
				string ret = serialize<B>(res);
				return move(ret);
			}
			case 4:
			{
				A para = deserilize<A>(buf);
				B res = m_xor(para);
				string ret = serialize<B>(res);
				return move(ret);
			}
			case 5:
			{
				C para = deserilize<C>(buf);
				B res = all_sum(para);
				string ret = serialize<B>(res);
				return move(ret);
			}
			case 6:
			{
				D res = printcat();
				string ret = serialize<D>(res);
				return move(ret);
			}
			default:
				return "";
		}
	}
	catch (string e)
	{
		cerr << e << "\n";
		return "";
	}
}
