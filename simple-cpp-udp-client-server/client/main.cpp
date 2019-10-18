#include <iostream>
#include <exception>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"
using namespace std;
/*
 * 服务端接受 Date 或 Time, 返回客户端日期（年月日）或（时分秒）
 * references https://blog.csdn.net/u011467458/article/details/52585457
 * 没有做参数合法性检查。
 */
int main(int argc,char *argv[]) //增加可以读取terminal参数
{
	string ip = "127.0.0.1";
	unsigned int port = 12003;
	client c = client("127.0.0.1", 12003);
	if (argc == 3)
	{
		string ip(argv[1]); // 检查，或者报错
		int port = atoi(argv[2]); // TODO: 增加检查
		c.reset(ip, port);  //
	} else if(argc != 1) {
		string name(argv[0]);
		cout<<" ------------- Wrong args format ------------ "<<endl;
        cout<<" ------- Usage: "<<name<<" <ip> <port> ------ "<<endl;
        cout<<" ---------  Now, using default config-------- "<<endl;
	} 
		cout<<"Client will serve at ip: "<<ip<<", with target port: "<<port<<endl;
	//EOF (Ctrl + D 结束运行。)
	while(true) {
		cout<<"Key in your cmd to the server: ";
		string cmd;
		cin>>cmd;
		if (cmd == "")
		{
			cout<<"EOF, exiting...\n";
			return 0;
		}
		c.sendCmd(cmd);
	}
}
