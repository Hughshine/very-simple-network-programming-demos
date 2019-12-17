#include "udpsocket.h"
// https://blog.csdn.net/qq_29344757/article/details/71616748
struct timeval timeout = {1, 0}; // 1s

UdpSocket::UdpSocket(string ip, unsigned port, UdpSocketType type)
{   
    this->_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) < 0)
    {
        error("setsockopt()");
    }
    this->type = type;
    if (_socket < 0)
    {
        error("socket()");
    }
    if (type == SERVER_SOCKET) {
        this->my_ip = ip;
        this->my_port = port;
    } 
    else if (type == CLIENT_SOCKET) {
        this->target_ip = ip;
        this->target_port = port;
    }
    setAddr();
}

string UdpSocket::get_target_ip()
{
    return target_ip;
}
unsigned UdpSocket::get_target_port()
{
    return target_port;
}

void UdpSocket::setAddr()
{
    if(type == CLIENT_SOCKET) {
        memset(&target_addr, 0, sizeof(target_addr));
        target_addr.sin_family = AF_INET;
        target_addr.sin_addr.s_addr = inet_addr(this->target_ip.c_str());
        target_addr.sin_port = htons(this->target_port);
    }
    if(type == SERVER_SOCKET) {
        memset(&my_addr, 0, sizeof(my_addr));
        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr = inet_addr(this->my_ip.c_str());
        my_addr.sin_port = htons(this->my_port);
    }
}
// https://xz.aliyun.com/t/3078
int UdpSocket::bind()
{
    if(type != SERVER_SOCKET) {
        cerr << "Clients are not allowed to bind() in our cases." << endl;
        return -1;
    }
    return testBind();
}

/**
 * SERVER_SOCKET should recv() first. 
 */
int UdpSocket::send(string msg)
{
    socklen_t addr_size = sizeof(target_addr);
    const char *str = msg.c_str();
    if(sendto(this->_socket, str, strlen(str) + 1, 0, (struct sockaddr *)&target_addr, addr_size)<0)
    {
        error("sendto()");
    }
    return 0;
}

/**
 * specify a target_addr; 
 */
int UdpSocket::sendTo(string msg, string target_ip, unsigned target_port)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(target_ip.c_str());
    server_addr.sin_port = htons(target_port);
    if(sendto(_socket, msg.c_str(), strlen(msg.c_str()) + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error("sendto()");
    }
    return 0;
}

UdpSocket::~UdpSocket()
{
    this->close();
}

bool UdpSocket::recv(string& str)
{
    socklen_t target_addr_size = sizeof(target_addr);
    int len = recvfrom(_socket, read_buffer, 1024, 0, (struct sockaddr *)&target_addr, &target_addr_size);
    if(len < 0)
    {
        // cerr << "!!!timeout" << endl;
        return false;
    }
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(target_addr.sin_addr), ip, sizeof(target_addr));
    target_ip = ip;
    target_port = ntohs(target_addr.sin_port);
    // cout << "Client IP: " << ip << endl;
    // cout << "Client port: " << ntohs(target_addr.sin_port) << endl;
    str = string(read_buffer);
    memset(read_buffer, 0, sizeof(read_buffer));
    return true;
}

int UdpSocket::close()
{
    return ::close(_socket);
}

int UdpSocket::testBind(int times)
{
    int bind_code = -1;
    for (int i = 0; i < times; i++)
    {
        cout << " --------- Trying to bind socket, attempts: " << i + 1 << "/10. --------- " << endl;
        bind_code = ::bind(_socket, (struct sockaddr *)&my_addr, sizeof(my_addr));
        if (bind_code == 0)
        {
            cout << " --------- Bind success!!! --------- " << endl;
            return 0;
            break;
        }
        cout << "error code: " << errno << endl; // 48就是already in use的意思
        sleep(1);
    }
    if (bind_code != 0)
    {
        cout << " --------- Bind failed... --------- " << endl;
        cout << "bind code: " << bind_code << endl;
        cout << " --------- Bind failed... --------- " << endl;
        return -1;
    }
    return -1;
}

void UdpSocket::error(const char *m)
{
    perror(m);
}