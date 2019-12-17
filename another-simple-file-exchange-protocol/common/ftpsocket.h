#ifndef FTPSOCKET_H
#define FTPSOCKET_H
#include "pch.h"
#include "udpsocket.h"
#include "packet.h"
using namespace std;
namespace bf = boost::filesystem;
enum FtpCmdType
{
    CMD_GOOD,
    CMD_DISCONNECT,
    CMD_WRONG,
};



void printFtpInfo(ostream& out, string info);
void printFtpRetInfo(FtpRetCode code);


class FtpServer 
{
public:
    struct FtpClientState // 一个用户只有一个目录，没有下级文件夹
    {
        string username;
        string password;
        string authkey;
        string client_ip;
        unsigned client_port;
        unsigned last_sent_seq; // 期待下一个发送的包的序号
        unsigned last_sent_ackid; // 上一个发送的ack，也就是期待的下一个包
        unsigned timeout_count;
        Packet last_sent_pkt;

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
    // unordered_map<std::thread::id, FtpClientState> thread_map; // 因为onCallback()是无状态的，所以需要借助此
    unsigned connection_count = 0;
    UdpSocket cmdSocket;

    // static const int NUMBER_CONNECTIONS;

    void processFtpRequest(FtpClientState *client); //start a new thread with a udpsocket

    int onListDicContent(FtpClientState *state); // ls
    int onRecvFileFromClient(FtpClientState *state);
    int onSendFileToClient(FtpClientState *state);
    int onSayBye(FtpClientState *state);
    int onWrongCmd(FtpClientState *state); // 返回给client错误信息，结束连接

    int onSendRaw(FtpClientState *state);
    int onRecvRaw(FtpClientState *state);

    bool lookup(const string &username, const string& password);
    bool validate(string authkey, Packet pkt);

    bool recvValidPacket(FtpClientState* state, Packet& pkt); // 同时改变state，不返回ack包。
    bool sendValidPacket(FtpClientState* state, Packet& pkt);
    bool sendPureAckPacket(FtpClientState* state);
};

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
#endif