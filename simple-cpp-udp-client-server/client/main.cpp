#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
/*
服务端接受 Date 或 Time, 返回客户端日期（年月日）或（时分秒）
 */

/*
 * references https://blog.csdn.net/u011467458/article/details/52585457
 */
void error(const char *m){
	perror(m);
	exit(0);
}
   
int main()
{
	char buffer[512] = "";
    const char *s_addr = "127.0.0.1";
	int i_port = 12003;

	cout<<"Create socket\n";
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s < 0) { error("socket()"); }
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(s_addr);
	server_addr.sin_port = htons(i_port);

	cout<<"Trying connect\n";
    if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    	error("connect()");
    }
    cout<<"sizeof(buffer)-1: "<<sizeof(buffer)-1<<endl;
//	recv(s, buffer, sizeof(buffer)-1, 0);
    recv(s, buffer, 512, 0);
	cout<<"Message form server: "<< buffer<<endl;
    cout<<"Connect success!"<<endl;
    close(s);
	return 0;
}