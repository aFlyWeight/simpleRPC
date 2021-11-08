#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <arpa/inet.h>
#include<cstring>
#include <string>
#include <unistd.h>

#include "testdata.h"

using namespace std;

int main()
{
   //创建通信的socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket: ");
        return -1;
    }
    //链接服务器的ip和端口
    struct sockaddr_in  saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    inet_pton(AF_INET, "10.0.4.7", &saddr.sin_addr.s_addr);
    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("connect: ");
        return -1;
    }
    bool isexit = 0;
    while (1)
    {
        //发送数据
        string rettype;
        string paratype;
        string funcname;
        cout << "Please input return data type:" << endl;
        cin >> rettype;
        cout << "Please input parameter type: " << endl;
        cin >> paratype;
        cout << "Please input funcname: " << endl;
        cin  >> funcname;
        //先查询类型编号
        char flag = '0';
        int sr = rettype.size();
        int sp = paratype.size();
        int tymsglen = 2 * sizeof(sr) + sr + sp + 1;
        char* tymsg = (char*)malloc(tymsglen);
        char* pty = tymsg;
        memcpy(pty, &flag, sizeof(flag));
        pty += sizeof(flag);
        memcpy(pty, &sr, sizeof(sr));
        pty += sizeof(sr);
        memcpy(pty, reinterpret_cast<char*>(&rettype[0]), sr);
        pty += sr;
        memcpy(pty, &sp, sizeof(sp));
        pty += sizeof(sp);
        memcpy(pty, reinterpret_cast<char*>(&paratype[0]), sp);

        string testsenddata(tymsg);
        cout << "Be going to send data: " << testsenddata << endl;
        send(fd, tymsg, tymsglen, 0);
        cout <<"send data: "<< testsenddata << endl;

        //需要接受两次数据
        bool isexit = 0;
        for (int i = 0; i < 2; i++)
        {
            //接受从服务端返回的数据
            char recvbuf[1024];
            string msg;
            int rlen = recv(fd, recvbuf, sizeof(recvbuf), 0);
            cout << "rlen: " << rlen << endl;
            if (rlen > 0)
            {
                cout << recvbuf << endl;
                int pos = 0;
                while (pos < rlen)
                {
                    msg += recvbuf[pos];
                    pos++;
                }
                cout << "msg: " << msg << endl;
                char flag = msg[0];
                if (flag == '0')
                {
                    char* precbuf = reinterpret_cast<char*>(&msg[1]);
                    int irettype;
                    memcpy(&irettype, precbuf, sizeof(irettype));
                    precbuf += sizeof(irettype);
                    int iparatype;
                    memcpy(&iparatype, precbuf, sizeof(iparatype));
                    cout << "irettype: " << irettype << " iparatype: " << iparatype << endl;
                    //然后将返回类型，参数类型，函数名长度，函数名，参数长度，参数组合起来发送
                    string paradata = serialize_type_with_num(iparatype);
                    int funsz = funcname.size();
                    int parasz = paradata.size();
                    int funmsglen = 4 * sizeof(irettype) + funsz + parasz + 1;
                    char* funmsg = (char*)malloc(funmsglen);
                    char* pfmsg = funmsg;
                    flag = '1';
                    memcpy(pfmsg, &flag, sizeof(flag));
                    pfmsg += sizeof(flag);
                    memcpy(pfmsg, &irettype, sizeof(irettype));
                    pfmsg += sizeof(irettype);
                    memcpy(pfmsg, &iparatype, sizeof(iparatype));
                    pfmsg += sizeof(iparatype);
                    memcpy(pfmsg, &funsz, sizeof(funsz));
                    pfmsg += sizeof(funsz);
                    memcpy(pfmsg, reinterpret_cast<char*>(&funcname[0]), funsz);
                    pfmsg += funsz;
                    memcpy(pfmsg, &parasz, sizeof(parasz));
                    pfmsg += sizeof(parasz);
                    memcpy(pfmsg, reinterpret_cast<char*>(&paradata[0]), parasz);

                    cout << "funmsglen: " << funmsglen << endl;
                    send(fd, funmsg, funmsglen, 0);
                }
                else if (flag == '1')
                {
                    char* precbuf = &recvbuf[1];
                    //先读出返回值类型
                    int irettype;
                    memcpy(&irettype, precbuf, sizeof(irettype));
                    precbuf += sizeof(irettype);

                    cout << "irettype: " << irettype << endl;

                    //再读返回长度
                    int retsz;
                    memcpy(&retsz, precbuf, sizeof(retsz));
                    precbuf += sizeof(retsz);

                    //再将返回内容转化为string
                    string rpcresultstring(precbuf, retsz);
                    if (irettype == 1)
                    {
                        A a = deserilize<A>(rpcresultstring);
                        cout << "A: " << a.b << " " << a.c << endl;
                    }
                    else if (irettype == 2)
                    {
                        B b = deserilize<B>(rpcresultstring);
                        cout << "B: " << b.a << endl;
                    }
                    else if (irettype == 3)
                    {
                        C c = deserilize<C>(rpcresultstring);
                        cout << "C: " << endl;
                        int n = sizeof(c.a) / sizeof(c.a[0]);
                        for (int i = 0; i < n; i++)
                        {
                            cout << "C.a[i]" << " ";
                        }
                        cout << endl;
                    }
                    else if (irettype == 4)
                    {
                        D d = deserilize<D>(rpcresultstring);
                        cout << "D: " << d.ch << endl;
                    }
                }
                else
                {
                    cerr << "Has no such function\n" << endl;
                }
            }
            else if (ret == 0)
            {
                cerr << "Server has interupted!\n";
                isexit = 1;
                break;
            }
            else
            {
                perror("Receive: ");
                isexit = 1;
                break;
            }
        }
        if (isexit == 1)
            break;
    }
    close(fd);
    return 0;
}