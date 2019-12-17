#include "../common/common.h"
#include "clientconsole.h"
int main()
{
    ClientConsole console = ClientConsole();
    console.start();
    // UdpSocket udpSocket = UdpSocket("127.0.0.1", 12005, UdpSocket::CLIENT_SOCKET);
    // string ret;
    // while(true)
    // {
    //     while(!udpSocket.recv(ret))
    //         udpSocket.send("hello");
    //     cout << ret << endl;
    // }
}