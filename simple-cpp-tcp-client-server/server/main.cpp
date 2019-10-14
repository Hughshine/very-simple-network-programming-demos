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
#include "server.h"
using namespace std;

int main(int argc, char* argv[])
{
    string ip = "127.0.0.1";
    unsigned int port = 12003;
    server s = server(ip, port);
    if (argc == 3)
    {
        string ip(argv[1]); // 检查，或者报错
        int port = atoi(argv[2]); // TODO: 增加检查
        s.resetConfig(ip, port);  //
    } else if(argc != 1) {
        string name(argv[0]);
        cout<<" ------------- wrong args format ------------ "<<endl;
        cout<<" --------- usage: "<<name<<" <ip> <port> -------- "<<endl;
        cout<<" ------ using default: 127.0.0.1:12003 ------ "<<endl;
    } else
        cout<<"Server will serve at ip: "<<ip<<":"<<port<<endl;

    
    int bindcode = s.bindSocket();
    if (!bindcode)
    {
        while(true){

            cout<<"try again? y/n."<<endl;
            string cmd;
            cin>>cmd;
            if (cmd != "y")
            {
                cout<<"Bye..."<<endl;
                return 0;;
            }
            bindcode = s.bindSocket();
        }
    }
    s.listen();
    s.dealWithRequests();
}

// void error(const char *m){
//     perror(m);
//     exit(0);
// }

// const std::string currentDate() {
//     time_t     now = time(0);
//     struct tm  tstruct;
//     char       buf[80];
//     tstruct = *localtime(&now);
//     strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
//     return buf;
// }

// const std::string currentTime() {
//     time_t     now = time(0);
//     struct tm  tstruct;
//     char       buf[80];
//     tstruct = *localtime(&now);
//     strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
//     return buf;
// }

// int main() {
// //
//     const char *s_addr = "127.0.0.1";
//     int i_port = 12003;

//     printf("Create socket\n");
//     int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (s < 0) { error("socket()"); }
//     cout<<"socket: "<<s<<endl;

//     struct sockaddr_in server_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = inet_addr(s_addr);
//     server_addr.sin_port = htons(12003);
//     int bind_code = -1;

//     for(int i=0;i<10 ;i++) {
//         cout<<" --------- Trying to bind socket, attempts: "<< i+1<<"/10. --------- "<<endl;
//         bind_code = bind(s, (struct sockaddr *) &server_addr, sizeof(server_addr));
//         if (bind_code == 0)  {
//             cout<<" --------- Bind success!!! --------- "<<endl;
//             break;
//         } 
//         cout<<"error code: "<<errno<<endl;  // 48就是already in use的意思
//         sleep(1);
//     }

//     if (bind_code != 0)
//     {
//         cout<<" --------- Bind failed... --------- "<<endl;
//         // cout<<"error code: "<<errno<<endl;  // 48就是already in use的意思；sleep()是不是也会改变errno呀。。
//         cout<<"bind code: "<<bind_code<<endl;
//         return 0;
//     }
//     int listen_code = listen(s, i_port);
//     cout<<"listen code: "<<listen_code<<endl;  // 不知道什么时候监听会失败。
//     cout<<" --------- start listening --------- \n";
//     struct sockaddr_in clnt_addr;
//     socklen_t clnt_addr_size = sizeof(clnt_addr);
//     int clnt_sock;
//     char read_buffer[64] = "";

//     while (true) {
//         clnt_sock = accept(s, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
//         cout<<"clnt_sock: "<< clnt_sock<<endl;
//         recv(clnt_sock, read_buffer, 64, 0);
//         cout<<"Read Buffer: "<< read_buffer<<endl;
//         string ret = "";
//         if(!strcmp(read_buffer, "Date")) {
//             ret = currentDate();
//         }else if(!strcmp(read_buffer, "Time")) {
//             ret = currentTime();
//         }else {
//             ret = "Sorry, wrong command.";
//         }
// //向客户端发送数据
//         const char* str = ret.c_str();
//         cout<<"callback: "<<str<<endl;
//         cout<< "sizeof(str): "<<strlen(str)<<endl;
//         write(clnt_sock, str, strlen(str)+1);
//         close(clnt_sock);
//     }

// //关闭套接字
//    close(clnt_sock);
//    close(s);
//    cout<<" --- Bye --- \n";
//    return 0;
// }