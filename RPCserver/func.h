#pragma once
#include <iostream>

#include "type.h"

/*该文件中存储用于远程调用的函数，函数参数默认为struct类型*/

using namespace std;


B m_sum(A &a)
{
	B ret;
	ret.a = a.b + a.c;
	return move(ret);
}

B m_sub(A &a)
{
	B ret;
	ret.a = a.b - a.c;
	return move(ret);
}

B m_multi(A &a)
{
	B ret;
	ret.a = a.b * a.c;
	return move(ret);
}

B m_dvd(A& a)
{
	B ret;
	if (a.c != 0)
		ret.a = a.b / a.c;
	else
		ret.a = 0;
	return move(ret);
}

B m_xor(A &a)
{
	B ret;
	ret.a = a.b ^ a.c;
	return move(ret);
}


B all_sum(C &a)
{
	int n = sizeof(a.a) / sizeof(a.a[0]);
	B ret;
	ret.a = 0;
	for (int i = 0; i < n; i++)
	{
//		cout << " a.a[i]: " << a.a[i];
		ret.a += a.a[i];
	}
//	cout << "\n";
	cout << "ret.a: " << ret.a << endl;
	return move(ret);
}

D printcat(VD)
{
	string str("this is a cat.");
	cout << str <<endl;
	int n = str.size();
	D ret;
	memcpy(ret.ch, str.c_str(), n);
	return move(ret);
}



