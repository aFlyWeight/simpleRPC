#pragma once
#include "type.h"
#include "serialize.h"


void init(A &a)
{
	a.b = 1;
	a.c = 1;
}

void init(B &b)
{
	b.a = 1;
}

void init(C &c)
{
	int n = sizeof(c.a) / sizeof(c.a[0]);
	for (int i = 0; i < n; i++)
	{
		c.a[i] = i + 1;
	}
}

/**********ÓÃÓÚ²âÊÔ****************/
void print(C& c)
{
	int n = sizeof(c.a) / sizeof(c.a[0]);
	for (int i = 0; i < n; i++)
	{
		cout<< " c.a[i]: " <<c.a[i];
	}
	cout << endl;
}
/**********************************/

void init(D& d)
{
	d.ch[20] = { 0 };
}

string serialize_type_with_num(int i)
{
	switch (i)
	{
	case 1:
	{
		A a;
		init(a);
		string ret = serialize<A>(a);
		return move(ret);
	}
	case 2:
	{
		B b;
		init(b);
		string ret = serialize<B>(b);
		return move(ret);
	}
	case 3:
	{
		C c;
		init(c);
//		print(c);
		string ret = serialize<C>(c);
		return move(ret);
	}
	case 4:
	{
		D d;
		init(d);
		string ret = serialize<D>(d);
		return move(ret);
	}

	default:
		break;
	}
	return "";
}