#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include "pch.h"
using namespace std;
class UdpSocket 
{
public:
    enum UdpSocketType {
        SERVER_SOCKET,
        CLIENT_SOCKET
    };

public:
    UdpSocket(string ip, unsigned port, UdpSocketType type);
    ~UdpSocket();
    int bind();
    int send(string msg);
    int sendTo(string msg, string target_ip = "127.0.0.1", unsigned target_port = 12005);
    bool recv(string &str);
    int close();
    UdpSocketType type = CLIENT_SOCKET;

    string get_target_ip();
    unsigned get_target_port();
private:
    void setAddr();
    int testBind(int times=10);
    void error(const char *m);

    int _socket;
    struct sockaddr_in target_addr;
    struct sockaddr_in my_addr;

    string target_ip;
    unsigned target_port;
    string my_ip;
    unsigned my_port;

    char read_buffer[1024];
};
#endif