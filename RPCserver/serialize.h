#pragma once
#include<iostream>
#include<cstring>
#include<string>

using namespace std;

/*ʹ��ǿ������ת��ʵ�����л��ͷ����л���ֻ������c���͵�struct�ṹ*/

template<typename T> //T һ����pod(ԭc����)��struct���� 
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