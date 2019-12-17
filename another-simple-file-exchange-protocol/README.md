# [another-simple-file-exchange-protocol](https://github.com/Hughshine/simple-ftp-server)

Table of Contents
=================

   * [another-simple-ftp-protocol](#another-simple-ftp-protocol)
      * [Overview](#overview)
      * [Features](#features)
      * [Project details](#project-details)
         * [Dependency](#dependency)
         * [Compilation](#compilation)
      * [Usage](#usage)
         * [client](#client)
         * [server](#server)
         * [sample](#sample)
      * [Implementation details](#implementation-details)
         * [Project structure](#project-structure)
         * [class Packet](#class-packet)
         * [class FtpServer](#class-ftpserver)
            * [apply()](#apply)
            * [processFtpRequest()](#processftprequest)
            * [Reliable connection implementation: sendValidPacket() &amp; recvValidPacket](#reliable-connection-implementation-sendvalidpacket--recvvalidpacket)
         * [class FtpClient](#class-ftpclient)
         * [cli: class ClientConsole](#cli-class-clientconsole)
      * [Issues - ways to make this better](#issues---ways-to-make-this-better)
      * [Welcome to give issues or make pr!!!](#welcome-to-give-issues-or-make-pr)
      * [Some references](#some-references)
         * [Notes](#notes)
         * [Some good sample repos](#some-good-sample-repos)

Created by [gh-md-toc](https://github.com/ekalinin/github-markdown-toc)

## Overview

Supported operations: 

1. login
2. list
3. get

> `send` method has net been implemented yet.

## Features

1. Implement with pure udp.
2. Security: simple auth system.
3. Server: support demux in transport layer, serving multiple clients at a time.
4. Client: friendly cli.

Attention: only text file supported due to the implementation.

## Project details 

### Dependency

* [boost](https://www.boost.org/)

### Compilation

```zsh
cd server 
make
# in another terminal
cd client
make

# or after add `client.sh` and `server.sh`
make  # in project root
```

> you should create `client.sh` & `server.sh` in this way, and do not forget to `chmod 744`:

```zsh
cd /<absolute_path_to>/<client>or<server>
make clean
make
clear
make test 
```

## Usage

### `client`

```zsh
# /client
# make test
 < Welcome to another-simple-ftp-client.
 > h
another-simple-ftp-protocol[options]
Options:
        cnct <ip> <port>  Connect to the server
        dcnct             Disconnect to the server
        h | help          Print this help
        q | quit          Quit this program
*** after cnct ***
        lls           ls local folder
        rls           ls remote folder
        get <local_file_name> <remote_file_name>           get remote file
        # send <local_file_name> <remote_file_name>          send local file, not implemented
```

### `server`

```zsh
# /server
# make test
 --------- Trying to bind socket, attempts: 1/10. --------- 
 --------- Bind success!!! --------- 
```

### sample

**client:**
```
 < Welcome to another-simple-ftp-client.
 > cnct 127.0.0.1 12005
 < connecting to 127.0.0.1:12005
Your account: test 
Your password: pwd
authkey: wQj99nsQzldV
 < `cnct` success!
 > lls
./user/test/
 .DS_Store  .gitignore
 > rls
.DS_Store       .gitignore      sample_large.txt        sample_small.txt
[FTP]: Success!
 > get s.txt sample_small.txt  
recved pkt: 1154
[FTP]: Success!
 > lls
./user/test/
 .DS_Store  .gitignore     s.txt
 > q
 < Bye.
```

**server log:**
```
./main
 --------- Trying to bind socket, attempts: 1/10. --------- 
 --------- Bind success!!! --------- 
get one pkt!
[One auth success!!!]
[wQj99nsQzldV] connected
FtpClientState : [username]:test, [password]:pwd [authkey]:wQj99nsQzldV
        [client_ip]: 127.0.0.1 [client_port]: 53449
        [last_sent_seq]: 1 [last_sent_ackid]: 1
        [timeout]: 0
FtpClientState : [username]:test, [password]:pwd [authkey]:wQj99nsQzldV
        [client_ip]: 127.0.0.1 [client_port]: 53449
        [last_sent_seq]: 1 [last_sent_ackid]: 1
        [timeout]: 0
cmd: ls
./user/test/
FtpClientState : [username]:test, [password]:pwd [authkey]:wQj99nsQzldV
        [client_ip]: 127.0.0.1 [client_port]: 53449
        [last_sent_seq]: 2 [last_sent_ackid]: 3
        [timeout]: 0
cmd: get
!!opening..../user/test/sample_small.txt send file: sample_small.txt ok!
packet count: 1154
FtpClientState : [username]:test, [password]:pwd [authkey]:wQj99nsQzldV
        [client_ip]: 127.0.0.1 [client_port]: 53449
        [last_sent_seq]: 1157 [last_sent_ackid]: 1158
        [timeout]: 0
[wQj99nsQzldV] disconnected
```

## Implementation details

### Project structure

```zsh
.
├── Makefile
├── README.md
├── client
│   ├── Makefile
│   ├── clientconsole.[h.cpp] # client cli
│   ├── main.cpp
│   └── user    # local user folder
│       └── test # test user
│           └── s.txt
├── client.sh
├── common
│   ├── common.h    
│   ├── ftpsocket.[h,cpp]   # ftp server, client definition
│   ├── packet.[h,cpp]    # protocol packet definition
│   ├── pch.h   # for standard lib precompilition 
│   └── udpsocket.[h.cpp]   # encapsulated linux udpsocket
├── server
│   ├── Makefile
│   ├── main
│   ├── main.cpp
│   └── user    # remote user folder
│       └── test # test user
│           ├── sample_large.txt
│           ├── sample_small.txt
│           └── userinfo    # auth
│               └── password.txt
└── server.sh
```

### class `Packet`

The packet is defined as following. It has 12 fields, and is seperated by ' ' in the real udp packet.

```cpp
    static const int APPID = 0;  // Protocol identifer in every valid packet. 
    static const int SEQ = 1;
    static const int ISACK = 2;
    static const int ACKID = 3;
    static const int ISPSH = 4; // Make the final data packet .
    static const int ISFIN = 5;
    static const int RET_CODE = 6;  // server's ret code
    static const int CMD_CODE = 7;  // client's cmd
    static const int USERNAME = 8;  // connection
    static const int PASSWORD = 9;  // connection
    static const int AUTHKEY = 10;  // auth
    static const int BODY = 11;     // real data
```

Some util functions are implemented, to change a `Packet` into a string or backwards.

### class `FtpServer`

Let's dive into the definition.
```cpp
class FtpServer 
{
public:
    struct FtpClientState // records for every connection
    {
        string username;
        string password;
        string authkey;
        string client_ip;
        unsigned client_port;
        unsigned last_sent_seq; 
        unsigned last_sent_ackid; 
        unsigned timeout_count;
        Packet last_sent_pkt;   // for retransmition
        string filename;
        UdpSocket* cnctSocket;
        FtpClientState(string username, string password, string client_ip, unsigned client_port);
        void print();
    };

public:
    FtpServer(unsigned bind_port);
    ~FtpServer();
    int apply();
private:
    unsigned connection_count = 0; // restrict the upper bound of connections
    UdpSocket cmdSocket;
    void processFtpRequest(FtpClientState *client); // each connection has its own thread

    int onListDicContent(FtpClientState *state); 
    int onRecvFileFromClient(FtpClientState *state); // not implemented
    int onSendFileToClient(FtpClientState *state);
    int onSayBye(FtpClientState *state);
    int onWrongCmd(FtpClientState *state); // 返回给client错误信息，结束连接

    // int onSendRaw(FtpClientState *state);
    // int onRecvRaw(FtpClientState *state);

    bool lookup(const string &username, const string& password); // check username, password
    bool validate(string authkey, Packet pkt); // validate a pkt

    bool recvValidPacket(FtpClientState* state, Packet& pkt); // recv a pkt according to its ackid/seq, if invalid, retransimit. if success, adjust state.
    bool sendValidPacket(FtpClientState* state, Packet& pkt); // send a pkt with correct seq & ackid 
    bool sendPureAckPacket(FtpClientState* state); // ack a data pkt, useless for server
};
```

#### `apply()`

This is a server-wise function, and it is used to validate each auth action from client and allocate a new thread to maintain the connection with it.
```cpp
int FtpServer::apply()
{
    cmdSocket.bind();
    string spkt;
    while (this->connection_count <= NUMBER_CONNECTIONS)
    {
        if(!cmdSocket.recv(spkt))
        {
            continue;
        }  
        Packet pkt = Packet(spkt);
        cout << "get one pkt!" << endl;
        if(pkt._appid != Packet::appid || pkt.username == "" || pkt.password == ""){
            continue;
        }
        string username = pkt.username;
        string password = pkt.password;
        if(!lookup(username, password))
        {
            Packet refuse_pkt = Packet(pkt.ackid, true, pkt.seq, FTP_UNAUTH);
            cmdSocket.send(refuse_pkt.make()); // 这里不维持连接，也不管这个包是否被ack。
            cout << "[One auth failed...]" << endl;
            continue;
        }
        cout << "[One auth success!!!]" << endl;
        FtpClientState *state = new FtpServer::FtpClientState(username, password, cmdSocket.get_target_ip(), cmdSocket.get_target_port());
        state->last_sent_seq = pkt.ackid;
        state->last_sent_ackid = pkt.seq;
        cout << "[" + state->authkey + "] connected" << endl;
        thread *_th = new std::thread(&FtpServer::processFtpRequest, this, state);
        this->connection_count++;
    }
    return 0;
}
```

#### `processFtpRequest()`

This function is the most important one. It's a client-wise function. After authorization, the server allocate a new `UdpSocket` to maintain the connection, which will send back the auth-ack packet first.
```cpp
void FtpServer::processFtpRequest(FtpServer::FtpClientState *state)
{
    // 0. get a new udpsocket, onAuth(), return a packet with retCode + authkey + ack
    Packet success_pkt = Packet(0, true, 1, FTP_GOOD, FTP_NONE, "", state->authkey);
    state->last_sent_seq++;
    state->last_sent_ackid++; 
    UdpSocket* cnctSocket = new UdpSocket(state->client_ip, state->client_port, UdpSocket::CLIENT_SOCKET); 
    state->cnctSocket = cnctSocket;
    cnctSocket->send(success_pkt.make());
    state->print();
    state->last_sent_pkt = success_pkt;
    Packet rpkt;
    while(true)
    {
        state->print();
        if (!this->recvValidPacket(state, rpkt)) // Waiting a cmd pkt for 100 seconds.
        {
            cerr << "连接中断" << endl;
            delete (state);
            delete (cnctSocket);
            this->connection_count--;
            return;
        }
        if(rpkt.is_fin)
        {
            cout << "[" << state->authkey << "] disconnected" << endl;
            delete (state);
            delete (cnctSocket);
            return;
        }
        int ret;
        switch(rpkt.cmd_code) // go to the function according to the cmd code.
        {
            case FTP_LIST:
                cout << "cmd: ls" << endl;
                ret = onListDicContent(state);
                break;
            // case FTP_SEND_TO_SERVER: // note implemented
            //     state->filename = rpkt.body;
            //     cout << "cmd: send" << endl;
            //     ret = onRecvFileFromClient(state);
            //     break;
            case FTP_GET_FROM_SERVER:
                state->filename = rpkt.body;
                cout << "cmd: get" << endl;
                ret = onSendFileToClient(state);
                break;
            case FTP_NONE:
                cerr << "A useless packet..." << endl;
                // send back its ack
                break;
            default:
                ret = onWrongCmd(state);
                cout << "a wrong cmd" << endl;
                break;
        }
        if(ret == CMD_DISCONNECT)
        {
            delete (state);
            delete (cnctSocket);
            this->connection_count--;
            return;
        }
    }
    this->connection_count--;
    return;
}
```

#### Reliable connection implementation: `sendValidPacket()` & `recvValidPacket`

This is the core of udp implemented ftp. The implementation is similar in the `FtpClient` too.
```cpp
/**
 * I set the correct value to the packet according to the state,
 * and record the `last_sent_pkt`
 */ 
bool FtpServer::sendValidPacket(FtpServer::FtpClientState *state, Packet &pkt)
{
    UdpSocket* socket = state->cnctSocket; 
    pkt.seq = ++state->last_sent_seq;
    pkt.is_ack = true;
    pkt.ackid = ++state->last_sent_ackid;
    pkt.authkey = state->authkey;
    state->last_sent_pkt = pkt;
    socket->send(pkt.make());
    return true;
}
/**
 * Hey! I will continuously get pkt, validate pkt, retransmit pkt for you,
 * until I get the corrent pkt you want.
 */ 
bool FtpServer::recvValidPacket(FtpServer::FtpClientState *state, Packet &pkt)
{
    UdpSocket* cnctSocket = state->cnctSocket;
    string ret;
    while(true)
    {
        if(!cnctSocket->recv(ret))
        {
            state->timeout_count++;
            if(state->timeout_count >= MAX_TIMEOUT)
            {
                cout << "recvValidPacket() timeout." << endl;
                return false;
            }
            continue;
        }
        Packet rpkt = Packet(ret);
        // cout << "收到了包" << endl;
        if(!this->validate(state->authkey, rpkt))
        {
            rpkt.print();
            cout << "!!!!收到无效包" << endl;
            state->print();
            rpkt.print();
            // return false;
            continue;
        }
        // 不是下一个包，重传
        if(rpkt.ackid < state->last_sent_seq  // 两者应是等价的
        || rpkt.seq < state->last_sent_ackid)
        {
            cout << "!!!![重传]" << endl;
            cnctSocket->send(state->last_sent_pkt.make());
            state->print();
            rpkt.print();
            continue;
        }
        pkt = rpkt;
        state->last_sent_seq = rpkt.ackid-1;
        state->last_sent_ackid = rpkt.seq;
        state->timeout_count = 0;
        return true;
    }
    return true;
}
```

### class `FtpClient`

Looks nearly the same with the server. And it's much simpler, all it has to do is to send the correct packet.
```cpp
class FtpClient
{
public:

public:
    FtpClient(string username, string password, string ip, string port);
    ~FtpClient();
    int apply();

    FtpRetCode sendAuth();
    FtpRetCode sendRls(ostream &out);
    FtpRetCode sendFile(string lfile, string rfile);
    FtpRetCode getFile(string lfile, string rfile);
    FtpRetCode sendFin();
private:
    string username;
    string password; //
    string authkey; // use this since success.

    string server_ip;
    string server_port;

    unsigned last_sent_seq = 0;  // 自己要发的下一个序号
    unsigned last_sent_ackid = 0;  // 上一次ack的对方的包
    Packet last_sent_pkt;

    unsigned timeout_count;

    UdpSocket* cmdSocket;

    int onWrongCmd(); // stdout错误信息，结束连接
    int onSendRaw();
    int onRecvRaw();

    bool validate(string authkey, Packet pkt);

    bool recvValidPacket(Packet& pkt);
    bool sendValidPacket(Packet& pkt);
    bool sendPureAckPacket();
};
```

### cli: class `ClientConsole`

```cpp
class ClientConsole 
{
public:
    enum CommandType
    {
        COMMAND_QUIT,
        COMMAND_INVALID,
        COMMAND_OK,
        COMMAND_DISCONNECT
    };

public:
    ClientConsole();
    ~ClientConsole(){};
    void start();
private:
    bool isConnected;

    string username;
    string password;
    FtpClient* client;

    CommandType handleCommand(string);
    // command list
    CommandType onConnect(const vector<string> &); // connect the server and auth
    CommandType onHelp(); // print help info
    // each client cannot make dir for simplification
    CommandType onLLS(const vector<string> &); //  list local file
    CommandType onRLS(const vector<string> &); //  list remote file
    CommandType onGetFile(const vector<string> &);
    // CommandType onSendFile(const vector<string> &); not implemented
    void giveInfo(string);  // print info to the user
};
```

## Issues - ways to make this better

1. Change the encapsulation of `UdpSocket` to support binary file.
2. Optimize the reliable transfer machanism.
3. Implement `send` method.
4. Encryption.
5. Clear the annoying warnings.
6. ...

***

## Welcome to give issues or make pr!!!

> In fact, I'm a new coder welcoming any friendly discussion :-)!!

## Some references

### Notes

1. [c++: the order of initialization list](https://stackoverflow.com/questions/30364585/will-be-initialized-after-wreorder)

2. Be cauious with the packeting framing(in ip level), any buffer(size)。

3. Use `htons()` & `ntohs()` to switch the port representation.( which waste me lots of time :-( ) 

### Some good sample repos

1. [tftpx](https://github.com/ideawu/tftpx), my roommates like it :-)
2. and, [very-simple-file-exchange-protocol](https://github.com/SunriseFox/very-simple-file-exchange-protocol) in Qt!