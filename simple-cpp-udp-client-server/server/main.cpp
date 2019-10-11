#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>
#include <time.h>
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

const std::string currentDate() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

    return buf;
}

const std::string currentTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);

    return buf;
}

int main() {
//
    const char *s_addr = "127.0.0.1";
    int i_port = 12003;

    printf("Create socket\n");
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) { error("socket()"); }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(s_addr);
    server_addr.sin_port = htons(12003);
    bind(s, (struct sockaddr *) &server_addr, sizeof(server_addr));
    listen(s, i_port);

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock;
    while (true) {
        clnt_sock = accept(s, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
        cout << clnt_sock << endl;
//向客户端发送数据
        string ret = currentDate() + " "+ currentTime();
        const char* str = ret.c_str();
        cout<<"current time:"<<str<<endl;
        cout<< "sizeof(str): "<<strlen(str)<<endl;
        write(clnt_sock, str, strlen(str));
    }

//关闭套接字
//    close(clnt_sock);
//    close(s);
//
//    printf("Connect success!\n");
//    close(s);
//    return 0;
}