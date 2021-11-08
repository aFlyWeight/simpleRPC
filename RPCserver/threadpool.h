#pragma once
#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<cstdlib>
#include<string>
#include <atomic>
#include <utility>
#include <queue>

using namespace std;

template <typename T>
struct task {					//任务类
	using  func = void *(*)(T*);   //函数指针
	func f;
	T* arg;
	int index;
	task() :f(nullptr), arg(nullptr), index(0) {}
	task(func fu, T* a)
	{
		f = fu;
		arg = a;
		index = 0;
	}
};

template <typename T>
class taskqueue {
public:
	void addtask(task<T> t)			//往任务队列中添加任务
	{
		lock_guard<mutex> lck(_tasklock);
		_taskq.push(move(t));
	}
	task<T> gettask()
	{
		lock_guard<mutex> lck(_tasklock);
		task<T> tmp;
		if (!_taskq.empty())
		{
			tmp = move(_taskq.front());
			_taskq.pop();
		}
		return move(tmp);
	}
	int getasknum()
	{
		lock_guard<mutex> lck(_tasklock);
		return _taskq.size();
	}
	bool isempty()
	{
		lock_guard<mutex> lck(_tasklock);
		return _taskq.empty();
	}
private:
	mutex _tasklock;
	queue<task<T>> _taskq;
};


template <typename T>
class threadpool {
public:
	threadpool(int a = 3, int b = 10);
	~threadpool();
	void addtask(task<T> t)
	{
		if (shutdown == 1)
			return;
		iolock.lock();
		cout << "Add task " << t.index << "." << endl;
		iolock.unlock();
		taskq.addtask(move(t));
		conTaskEmpty.notify_one();
	}
	int getalivenum()
	{
		return livenum;
	}

private:
	static void* worker(threadpool* p);
	static void* manager(threadpool* p);

private:
	int minum;
	int maxnum;
	taskqueue<T> taskq;    //任务队列

	thread* workerIDs;	//工作线程数组
	thread managerID;         //管理线程
	atomic_bool shutdown;      //是否关闭线程池
	atomic_int busynum;		//忙碌线程数量
	atomic_int livenum;		//总共线程数量（存活的线程数）
	atomic_int exitnum;   //退出线程数量

	mutex mu;             //锁 用于条件变量
	mutex iolock;          //用于互斥打印输入输出
	condition_variable conTaskEmpty; //任务队列空条件变量

	const int canclenum = 2;
	const int addnum = 1;     //一次增加的线程数量
};


template <typename T> threadpool<T>::threadpool(int a, int b)
{
	minum = a;
	maxnum = b;
	shutdown = 0;
	busynum = 0;
	livenum = minum;
	exitnum = 0;
	try {
		workerIDs = new thread[maxnum];      // 申请线程队列空间
		if (workerIDs == nullptr)
		{
			throw - 1;
		}
		for (int i = 0; i < minum; i++)    //刚开始启动最小数量的线程
		{
			workerIDs[i] = thread(worker, this);
			iolock.lock();
			cout << "Starting initial worker thread " << workerIDs[i].get_id() << endl;
			iolock.unlock();
		}
		managerID = thread(manager, this);     //启动管理者线程
		iolock.lock();
		cout << "Starting manager thread " << managerID.get_id() << endl;
		iolock.unlock();
	}
	catch (int e)
	{
		if (e == -1)
		{
			cout << "Threadqueue application failed. " << endl;
		}
	}
}


template <typename T> threadpool<T>::~threadpool()
{
	shutdown = 1;

	if (managerID.joinable())      //回收管理线程
		managerID.join();
	iolock.lock();
	cout << "Threadpool is shutting down..." << " Manager " << managerID.get_id() << " will exit..." << endl;
	iolock.unlock();

	conTaskEmpty.notify_all();    //激活所有空闲线程之后，他们会自杀

	for (int i = 0; i < maxnum; i++)	//回收工作线程
	{
		if (workerIDs[i].joinable())
		{
			workerIDs[i].join();
			iolock.lock();
			cout << " Threadworker " << workerIDs[i].get_id() << "exits" << endl;
			iolock.unlock();
		}
	}
	delete[]workerIDs;
	iolock.lock();
	cout << "All works are done!" << endl;
	iolock.unlock();
}

template <typename T>
void* threadpool<T>::worker(threadpool* pool)
{
	while (1)
	{
		unique_lock<mutex> unithdlck(pool->mu);
		while (pool->taskq.isempty() && pool->shutdown != 1)
		{
			pool->iolock.lock();
			cout << "thread " << this_thread::get_id() << " is blocked since the taskqueue is empty." << endl;
			pool->iolock.unlock();
			pool->conTaskEmpty.wait(unithdlck);
			if (pool->exitnum > 0)    // 被强制唤醒之后线程自杀
			{
				pool->exitnum--;
				if (pool->livenum > pool->minum)
				{
					pool->livenum--;
					pool->iolock.lock();
					cout << "thread " << this_thread::get_id() << " is cancled compulsorily by manager." << endl;
					pool->iolock.unlock();
					unithdlck.unlock();
					return nullptr;
				}
			}
		}
		if (pool->taskq.isempty() && pool->shutdown == 1)
		{
			pool->iolock.lock();
			cout << "Threadpool is shutting down..." << "Worker thread " << this_thread::get_id() << "  will  exit." << endl;
			pool->iolock.unlock();
			pool->livenum--;
			unithdlck.unlock();
			return nullptr;
		}

		task<T> t = pool->taskq.gettask();

		unithdlck.unlock();

		pool->busynum++;

		pool->iolock.lock();
		cout << "thread " << this_thread::get_id() << " is working on task " << t.index << endl;
		pool->iolock.unlock();

		if (t.f != nullptr)
		{
			if (t.arg != nullptr)
			{
				t.f(t.arg);
//				delete t.arg;
//				t.arg = nullptr;
			}
			else
			{
				t.f(nullptr);
			}
		}
		this_thread::sleep_for(chrono::seconds(2));
		pool->iolock.lock();
		cout << "Existing " << pool->taskq.getasknum() << " tasks." << endl;
		pool->iolock.unlock();
		pool->busynum--;
	}
}

template <typename T>
void* threadpool<T>::manager(threadpool* pool)
{
	while (pool->shutdown == 0)
	{
		int b_num = pool->busynum;
		int lnum = pool->livenum;
		int tasknum = pool->taskq.getasknum();
		pool->iolock.lock();
		cout << "Supervising threads. " << "Living thread's num " << lnum << ". Busy thread's num " << b_num << "." << endl;
		pool->iolock.unlock();

		if (lnum > pool->minum && 2 * b_num < lnum)     //撤销线程
		{
			pool->exitnum = pool->canclenum;
			for (int i = 0; i< pool->canclenum && pool->livenum > pool->minum; i++)
			{
				pool->conTaskEmpty.notify_one();
			}
		}
		int taddnum = pool->addnum;
		if (lnum < pool->maxnum && lnum < tasknum)			//当前线程数小于最大线程数并且当前线程数小于任务数的时候添加线程
		{
			for (int i = 0; i < pool->maxnum && taddnum>0; i++)
			{
				if (pool->workerIDs[i].get_id() == thread::id())
				{
					pool->workerIDs[i] = thread(worker, pool);

					pool->iolock.lock();
					cout << "Add worker thread " << pool->workerIDs[i].get_id() << endl;
					pool->iolock.unlock();

					pool->livenum++;
					taddnum--;
				}
			}
		}
		this_thread::sleep_for(chrono::seconds(5));
	}
	return nullptr;
}