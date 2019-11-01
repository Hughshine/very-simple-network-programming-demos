#include "common.h"
#ifndef STATUS_OK
#define STATUS_OK false
#define STATUS_FAIL true
#endif
#define UNINITIALIZED 0
#define WELCOME_SOCKET 1
#define CONNECT_SOCKET 2

using namespace std;

class TCPSocket
{
public:
    TCPSocket(unsigned bind_port);
    ~TCPSocket(){::close(_socket);successMsg("close()");}
    int listen();
    int bind();
    TCPSocket* accept();
    string rcv();
    // char* rcvRaw();
    int send(string msg);//connected, ok
    int send(char *buf, int size);
    int close();
    void setSocketAndClientAddr(int _socket, unsigned bind_port, string ip = "127.0.0.1");

private:
    void successMsg(string msg);
    void errorMsg(string msg);
    int SOCKET_TYPE = UNINITIALIZED;
    int set(unsigned client_port , string ip = "127.0.0.1");
    int testBind(int times=10);
    int _socket;
    bool binded = false;
    bool connected = false;
    unsigned bind_port;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char read_buffer[1024];
};