#pragma once
#include<iostream>
#include<cstring>
#include<string>

using namespace std;

/*使用强制类型转换实现序列化和反序列化，只能用于c类型的struct结构*/

template<typename T> //T 一般是pod(原c语言)的struct类型 
string serialize(T& message)
{
	auto ptr = reinterpret_cast<char*>(&message);
	string ret(ptr, ptr + sizeof(message));
	return move(ret);
}

template<typename T>
T deserilize(string& buf)
{
	T ret;
	auto bufsize = buf.size();
	auto retsize = sizeof(ret);
	if (retsize != bufsize)
		throw string("The function parameters are inconsistent.");
	auto ptr = reinterpret_cast<char*>(&buf[0]);
	memcpy(&ret, ptr, bufsize);
	return move(ret);
}