#pragma once
#include <map>
#include <iostream>
#include <queue>

using namespace std;

//函数和类型绑定类都是单例模式

//将函数名和函数地址进行绑定
//绑定函数默认是 functype 类型的，因此绑定前需要进行类型转化
//因为增加删除函数或类型都必须重新启动，因此没有必要解出绑定
class funcmap {
public:
	static funcmap& getInstance()
	{
		static funcmap instance;
		return instance;
	}
	void bind(string name)
	{
		if (_fmap.count(name) != 0)
		{
			cerr << "The function " << name << " has been bind.\n";
			return;
		}
		_fmap.insert(make_pair(name,index++));
	}
	//void unbind(string name)         
	//{
	//	if (_fmap.count(name) == 0)
	//	{
	//		cerr << "Can not find function with name: "<< name<< "\n";
	//		return;
	//	}
	//	_fmap.erase(name);
	//}
	int getfunc(string &name)
	{
		cout << "in gefunc name: " << name << endl;
		if (_fmap.count(name) == 0)
		{
			cerr << "Can not find function with name: " << name << "\n";
			return -1;
		}
		return _fmap[name];
	}
	void print()
	{
		for (auto it = _fmap.begin(); it != _fmap.end(); it++)
			cout << it->first << " " << it->second << endl;
	}
private:
	funcmap() { index = 0; };
	~funcmap() {};
	funcmap(const funcmap&) = delete;
	funcmap& operator=(const funcmap&) = delete;
	map<string,int> _fmap;   //用函数名和类型序号来确定一个函数的地
	int index;
};

//用于记录参数编号,对每一个类型的参数进行编号,从0开始
class paramap {
public:
	static paramap& getInstance()
	{
		static paramap instance;
		return instance;
	}
	void bind(string type)
	{
		if (_pmap.count(type) != 0)
		{
			cerr << "The type " << type << " has been bind.\n";
			return;
		}
		_pmap.insert(make_pair(type, index));
		_imap.insert(make_pair(index, type));
		index++;
	}
	/*void unbind(string type)
	{
		if (_pmap.count(type) == 0)
		{
			cerr << "Can not find type: " << type << "\n";
			return;
		}
		_pmap.erase(type);
	}*/
	int getindexbytype(string type)
	{
		if (_pmap.count(type) == 0)
		{
			cerr << "Can not find type: " << type << "\n";
			return -1;
		}
		return _pmap[type];
	}
	string gettypebyindex(int index)
	{
		if (_imap.count(index) == 0)
		{
			cerr << "Can not find type index: " << index << "\n";
			return NULL;
		}
		return _imap[index];
	}
private:
	paramap()
	{
		index = 0;
	}
	~paramap() {};
	paramap(const paramap&) = delete;
	paramap& operator=(const paramap&) = delete;
	map<string, int> _pmap;    //每一个类型名称
	map<int, string> _imap;    //建立双向映射
//	queue<int> _erasedID;      //用于存储已经删除了的类型的ID
	int index;
};
