#include "tcpsocket.h"

TCPSocket::TCPSocket(unsigned bind_port){
    set(bind_port);
}

int TCPSocket::close()
{
    ::close(this->_socket);
    this->SOCKET_TYPE = UNINITIALIZED;
    return STATUS_OK;
}

int TCPSocket::set(unsigned bind_port, string ip)
{
    this->bind_port = bind_port;
    // successMsg("set bind_port:" + to_string(bind_port));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(bind_port);
    return STATUS_OK;
}

// set to WELCOME_SOCKET
int TCPSocket::bind()
{
    this->SOCKET_TYPE = WELCOME_SOCKET;
    this->_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->_socket < 0)
    {
        errorMsg("Failed to create socket");
        return STATUS_FAIL;
    } 
    else
        successMsg("Creating socket [" + to_string(this->_socket) +"]");
    int status = testBind(10);
    if(!status){
        this->binded = true;
        return STATUS_OK;
    }
    return STATUS_FAIL;
}

int TCPSocket::testBind(int times)
{
    if (this->SOCKET_TYPE != WELCOME_SOCKET)
    {
        errorMsg("Only welcome_socket can call rcv().");
        return STATUS_FAIL;
    }
    int bind_code = -1;
    for (int i = 0; i < times; i++)
    {
        successMsg("Trying to bind socket, attempts: " + to_string(i+1) + " / " + to_string(times));
        bind_code = ::bind(_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bind_code == 0)
        {
            successMsg("Bind success! ");
            return 0;
            break;
        }
        errorMsg("Bind error! [errno]:" + to_string(errno));
        sleep(1);
    }
    // if (bind_code != 0)
    // {
    //     cout << " --------- Bind failed... --------- " << endl;
    //     cout << "bind code: " << bind_code << endl;
    //     cout << " --------- Bind failed... --------- " << endl;
    //     return 1;
    // }
    errorMsg("Bind Failed, TERMINATING");
    return STATUS_FAIL;
}

int TCPSocket::listen()
{
    if(this->SOCKET_TYPE != WELCOME_SOCKET)
    {
        errorMsg("Only welcome_socket can call rcv().");
        return STATUS_FAIL;
    }
    if (!binded)
    {
        errorMsg("Please bind your socket first before listening.");
        return STATUS_FAIL;
    }
    ::listen(this->_socket, this->bind_port);
    // int listen_code = ::listen(this->_socket, this->bind_port);
    // cout << "listen code: " << listen_code << endl; // 不知道什么时候监听会失败。
    successMsg("Start Listening...");
    return STATUS_OK; //TODO 一般不会出错
}
/**
 * 不应被用户explicit调用
 */
void TCPSocket::setSocketAndClientAddr(int _connect_socket, unsigned client_port, string ip)
{
    this->connected = true; // 一般调用情况是这样的
    this->_socket = _connect_socket;  // 连接应已经是建立好的了。
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    client_addr.sin_port = htons(bind_port);
    this->SOCKET_TYPE = CONNECT_SOCKET;
    successMsg("Created connection socket[" + to_string(_connect_socket) + "]");
}
/**
 * TODO: 只有作为welcome_socket才使用该方法
 * 返回一个初始化过的server_addr, client_addr的TCPSocket
 */
TCPSocket* TCPSocket::accept()
{
    if (this->SOCKET_TYPE != WELCOME_SOCKET)
    {
        errorMsg("Only welcome_socket can call rcv().");
        return nullptr;
    }
    socklen_t client_addr_size = sizeof(client_addr);
    int _connect_socket = ::accept(_socket, (struct sockaddr *)&client_addr, &client_addr_size);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, sizeof(client_ip));

    successMsg("Client IP: " + string(client_ip));
    successMsg("Client port: " + to_string(client_addr.sin_port));

    TCPSocket* connect_socket = new TCPSocket(this->bind_port);
    connect_socket->setSocketAndClientAddr(_connect_socket, client_addr.sin_port, client_ip);
    return connect_socket;
}

/**
 * connect_socket func
 */
string TCPSocket::rcv() 
{
    if(this->SOCKET_TYPE != CONNECT_SOCKET)
    {
        errorMsg("Only connect_socket can call rcv().");
        return NULL;
    }
    memset(&this->read_buffer[0], 0, sizeof(this->read_buffer));
    int closed = recv(_socket, this->read_buffer, 1024, 0);
    if(closed == 0)
    {
        successMsg("socket closed by client");
        return ""; // TODO: not sure with NULL
    }
    return string(this->read_buffer);
}

/**
 * connect_socket func
 */
int TCPSocket::send(string msg="hello") 
{
    if (this->SOCKET_TYPE != CONNECT_SOCKET)
    {
        errorMsg("Only connect_socket can call rcv().");
        return STATUS_FAIL;
    }
    const char* str = msg.c_str();
    size_t size = strlen(str);
    ::write(_socket, str, size);
    return STATUS_OK;
}

/**
 * connect_socket func
 * 若size<0, 传送长度使用strlen确定
 */
int TCPSocket::send(char* buf, int size=-1)
{
    if (this->SOCKET_TYPE != CONNECT_SOCKET)
    {
        errorMsg("Only connect_socket can call rcv().");
        return STATUS_FAIL;
    }
    if(size < 0) size=strlen(buf);
    ::write(_socket, buf, size);
    return STATUS_OK;
}

void TCPSocket::successMsg(string msg)
{
    cout << "Success msg from TCPSocket["<< this->_socket << "]: " << msg << endl;
}

void TCPSocket::errorMsg(string msg)
{
    cerr << "Something wrong with TCPSocket[" << this->_socket << "]: " << msg << endl;
}