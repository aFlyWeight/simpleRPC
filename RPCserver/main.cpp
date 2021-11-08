#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <map>
#include <string>
#include <fcntl.h>
#include<errno.h>

#include "threadpool.h"
#include "init.h"
#include "funccall.h"

using namespace std;

#define CREATESTRCUCT(type) struct type paras;

mutex maplock;

typedef struct sockinfo {
	int fd;
	int epfd;
	map<int, pair<string, int>>* pmpfdip;
}SockInfo;

void* acceptConn(SockInfo* arg)
{
	struct sockaddr_in caddr;
	int caddrlen = sizeof(caddr);
	int cfd = accept(arg->fd, (struct sockaddr*)&caddr, (socklen_t*)&caddrlen);
	if (cfd == -1)
	{
		perror("accept error: ");
	}
	//保存客户端的ip地址和端口
	char cip[32];
	inet_ntop(AF_INET, &caddr.sin_addr.s_addr, cip, sizeof(cip));  //将网络序转化为主机序
	int cport = ntohs(caddr.sin_port);
	string strip(cip);
	cout << "test*" << strip << endl;
//	cout << arg->pmpfdip->size() << endl;
	maplock.lock();
	arg->pmpfdip->insert(make_pair(cfd, make_pair(strip, cport)));
	maplock.unlock();
	cout << "test**" << endl;
	printf("Client IP: %s, port: %d has connected.\n", cip, cport);
	//因为采用边沿触发模式，所以需要多次读取接收缓存，
	// 因此recv函数读取该文件描述符的时候应该是非阻塞的。
	//将要建立连接的文件描述符设置成非阻塞状态
	int flag = fcntl(cfd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(cfd, F_SETFL, flag);
	// 将要建立连接的文件描述符加入epoll树中
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;    //采用边沿触发模式
	ev.data.fd = cfd;
	int ret = epoll_ctl(arg->epfd, EPOLL_CTL_ADD, cfd, &ev);    //这里ev的添加是拷贝，所以可以继续用之前的ev变量
	if (ret == -1)
	{
		perror("epoll_ctl add lfd :");
		return nullptr;
	}
	return nullptr;
}

void* communication(SockInfo* arg)
{
	//接收客户端发送的数据
	//接收到的第一个字节为0代表类型查询，为1代表函数调用
	char buff[1024];
	string msg;
	while (1)
	{
		int rlen = recv(arg->fd, buff, sizeof(buff), 0);
		if (rlen > 0)
		{
			//处理接收的消息
			//...
			cout << buff << endl;
			int pos = 0;
			while (pos < rlen)
			{
				msg += buff[pos];
				pos++;
			}
			
		}
		else if (rlen == 0)
		{
			maplock.lock();
			printf("Client IP: %s, port: %d has interupted.\n",
				(*(arg->pmpfdip))[arg->fd].first.c_str(),
				(*(arg->pmpfdip))[arg->fd].second);
			arg->pmpfdip->erase(arg->fd);
			maplock.unlock();
			epoll_ctl(arg->epfd, EPOLL_CTL_DEL, arg->fd, NULL);
			close(arg->fd);
			break;
		}
		else
		{
			if (errno == EAGAIN)
			{
				printf("All data has been received.\n");
				cout << "msg size: " << msg.size() << endl;
				char flag = msg[0];
				if (flag == '0')  //如果第一个字节为0，则说明查询类型
				{
					//先读出两个类型字符串
					auto ptr = reinterpret_cast<char*>(&msg[1]);
					int l1;
					memcpy(&l1, ptr, sizeof(int));
					ptr += sizeof(int);
					string type1(ptr, ptr + l1);
					ptr += l1;
					int l2;
					memcpy(&l2, ptr, sizeof(int));
					ptr += sizeof(int);
					string type2(ptr, ptr + l2);
					//进行给查询
					cout << "Search type1: " << type1 << endl;
					cout << "Search type2: " << type2 << endl;
					int itype1 = paramap::getInstance().getindexbytype(type1);
					int itype2 = paramap::getInstance().getindexbytype(type2);
					cout << "Searched type1: " << itype1 << endl;
					cout << "Searched type2: " << itype2 << endl;
					char* remsg = (char*)malloc((2 * sizeof(int) + 1));
					char* pr = remsg;
					memcpy(pr, &flag, sizeof(flag));
					pr += sizeof(flag);
					memcpy(pr, &itype1, sizeof(itype1));
					pr += sizeof(itype1);
					memcpy(pr, &itype2, sizeof(itype2));
					send(arg->fd, remsg, (2 * sizeof(int) + 1), 0);
					free(remsg);
				}
				else
				{
					//先读出参数类型和返回类型
					auto ptr = reinterpret_cast<char*>(&msg[1]);
					int rettype;
					memcpy(&rettype, ptr, sizeof(rettype));
					ptr += sizeof(rettype);
					int paratype;
					memcpy(&paratype, ptr, sizeof(paratype));
					ptr += sizeof(paratype);
					//读出函数名称长度
					int namelen;
					memcpy(&namelen, ptr, sizeof(namelen));
					ptr += sizeof(namelen);

					cout << "namelen: " << namelen << endl;

					char* funcname = (char*)malloc(namelen);
					memcpy(funcname, ptr, namelen);
					ptr += namelen;
					//获取函数键值
					string funckey = generate_funckey(rettype, paratype, funcname);
					//获取函数编码
					cout << "funckey: " << funckey << endl;
					int funcnum = funcmap::getInstance().getfunc(funckey);
					cout << "funcnum: " << funcnum << endl;
					free(funcname);
					//读取参数类型长度
					int paralen;
					memcpy(&paralen, ptr, sizeof(paralen));
					ptr += sizeof(paralen);
					cout << "paralen: " << paralen << endl;
					char* para = nullptr;
					if (paralen != 0)
					{
						para = (char*)malloc(paralen);
						memcpy(para, ptr, paralen);
					}
					string strpara(para, paralen);
					if (para != nullptr)
						free(para);
					if (funcnum != -1)    
					{
						A a;
						cout << "size of A: " << sizeof(a) << endl;
						cout << "strpara: "<< strpara << "size of strpara: " << strpara.size() << endl;

						string ret = funcCall(funcnum, strpara);
						int remsglen = ret.size() + 2*sizeof(rettype) + 1;
						char* remsg = (char*)malloc(remsglen);
						char* pr = remsg;
						memcpy(pr, &flag, sizeof(flag));
						pr += sizeof(flag);
						memcpy(pr, &rettype, sizeof(rettype));
						pr += sizeof(rettype);
						int retsize = ret.size();
						memcpy(pr, &retsize, sizeof(retsize));
						pr += sizeof(retsize);
						memcpy(pr, reinterpret_cast<char*>(&ret[0]), retsize);
						send(arg->fd, remsg, remsglen, 0);
						free(remsg);
					}
					else					//没有找到函数
					{
						char remsg = '3';
						send(arg->fd, &remsg, 1, 0);
					}
				}
			}
			else
			{
				perror("recv error: ");
			}
			break;
		}
	}
	return nullptr;
}


int main()
{
	//先加载类型和函数
	init();
	//先打印所有函数和他的编号
	funcmap::getInstance().print();


	//创建socket
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1)
	{
		perror("socket error: ");
		return -1;
	}
	//绑定ip地址和端口
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;   //ipv4
	saddr.sin_port = htons(9999);
	saddr.sin_addr.s_addr = INADDR_ANY;   //0.0.0.0 在服务端自动读取本地的ip地址
	int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));
	if (ret == -1)
	{
		perror("bind error:");
		return -1;
	}
	//监听
	ret = listen(lfd, 128); //可用于监听的最大连接数
	if (ret == -1)
	{
		perror("bind error:");
		return -1;
	}
	//创建epoll实例
	int epfd = epoll_create(1);
	if (epfd == -1)
	{
		perror("epoll create:");
		return -1;
	}
	//把监听的文件描述符添加到epoll树中
	struct epoll_event ev;
	ev.events = EPOLLIN|EPOLLET;
	ev.data.fd = lfd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	if (ret == -1)
	{
		perror("epoll_ctl add lfd :");
		return -1;
	}
	struct epoll_event evs[1024];
	int size = sizeof(evs) / sizeof(evs[0]);
	map<int, pair<string,int>> mpfdip;
	//创建线程池
	threadpool<SockInfo> pool(1, 1);
	while (1)
	{
		int readynum = epoll_wait(epfd, evs, size, -1);  //-1 代表阻塞
		printf("readynum = %d\n", readynum);
		for (int i = 0; i < readynum; i++)
		{
			int tfd = evs[i].data.fd;
			if (tfd == lfd)       //如果是监听的文件描述符，则将要建立连接的文件描述符加入epoll树中
			{
				SockInfo soc;
				soc.epfd = epfd;
				soc.fd = tfd;
				soc.pmpfdip = &mpfdip;

				//创建任务
				task<SockInfo> t(acceptConn, &soc);
				pool.addtask(t);					//将任务添加到线程池中
//				acceptConn(&soc);
//				soc.pmpfdip = nullptr;
			}
			else   //用于通信的文件描述符
			{
				SockInfo soc;
				soc.epfd = epfd;
				soc.fd = tfd;
				soc.pmpfdip = &mpfdip;

				//创建任务
				task<SockInfo> t(communication, &soc);
				pool.addtask(t);					//将任务添加到线程池中
//				communication(&soc);
//				soc.pmpfdip = nullptr;
			}

		}
	}
	close(lfd);
	return 0;
}
