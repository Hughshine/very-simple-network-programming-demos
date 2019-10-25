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
    s.dealWithRequests();
}