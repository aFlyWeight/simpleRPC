#pragma once
#include <map>
#include <iostream>
#include <queue>

using namespace std;

//���������Ͱ��඼�ǵ���ģʽ

//���������ͺ�����ַ���а�
//�󶨺���Ĭ���� functype ���͵ģ���˰�ǰ��Ҫ��������ת��
//��Ϊ����ɾ�����������Ͷ������������������û�б�Ҫ�����
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
	map<string,int> _fmap;   //�ú����������������ȷ��һ�������ĵ�
	int index;
};

//���ڼ�¼�������,��ÿһ�����͵Ĳ������б��,��0��ʼ
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
	map<string, int> _pmap;    //ÿһ����������
	map<int, string> _imap;    //����˫��ӳ��
//	queue<int> _erasedID;      //���ڴ洢�Ѿ�ɾ���˵����͵�ID
	int index;
};
