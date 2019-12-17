#ifndef PACKET_H
#define PACKET_H
#include "pch.h"
using namespace std;

enum FtpRetCode
{
    FTP_GOOD,
    FTP_UNAUTH,
    FTP_BAD_CMD,
    FTP_FILE_INEXIST,
    FTP_BAD_NETWORK,
    FTP_FILE_COVERED //成功传输文件，但覆盖
};

enum FtpCmdCode
{
    FTP_NONE,
    FTP_LIST,
    FTP_SEND_TO_SERVER,
    FTP_GET_FROM_SERVER
};
/**
 * Packet for another-simple-ftp-protocol
 */ 
class Packet 
{
public:
    static const int APPID = 0;
    static const int SEQ = 1;
    static const int ISACK = 2;
    static const int ACKID = 3;
    static const int ISPSH = 4;
    static const int ISFIN = 5;
    static const int RET_CODE = 6;
    static const int CMD_CODE = 7;
    static const int USERNAME = 8;
    static const int PASSWORD = 9;
    static const int AUTHKEY = 10;
    static const int BODY = 11; 

    static const string appid;
public:
    Packet(unsigned seq, bool is_ack, unsigned ackid, FtpRetCode code = FTP_GOOD, FtpCmdCode cmd_code = FTP_NONE, 
        string body = "", string authkey = "0", bool is_psh = false, bool is_fin = false, string username = "0", 
        string password = "0");
    // seq， ack，不手动配置
    Packet(FtpRetCode code, FtpCmdCode cmd_code,
           string body = "", string authkey = "0", bool is_psh = false, bool is_fin = false, string username = "0",
           string password = "0");
    Packet(string packet); // parse a string into pkt
    Packet(const Packet& packet);
    Packet();

    Packet& operator=(const Packet&packet);

    void print();   
    Packet ack(); // 产生一个ack这个包的单纯ack包。
    string make(); // make up to a string

    string _appid;  
    unsigned seq;
    bool is_ack;
    unsigned ackid; // 期待的下一个seq
    bool is_psh;
    bool is_fin;
    FtpRetCode ret_code;
    FtpCmdCode cmd_code;
    string username; // 只有第一个包才携带
    string password; // 只有第一个包才携带
    string authkey; // 之后都以此为准
    string body; // main info

private:
    static vector<string> get_header_and_body(string packet);
    static const int MAX_LENGTH = 512; // body最大长度
    static const int MAX_SEQ = 0x8000000;
};
#endif