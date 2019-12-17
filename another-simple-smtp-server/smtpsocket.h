#include "tcpsocket.h"
#include "common.h"
#define mem
#define func
#define def
#define NUMBER_CONNECTIONS 8
#define STATUS_OK false
#define STATUS_FAIL true
#define STATUS_FINISHED true

using namespace std;

class SMTPServer
{
private def : 
    struct Command
    {
        string command;
        std::function<bool(TCPSocket *, string)> callback;
    };

    struct SMTPClientState
    {
        string mailFrom;
        string recpTo;
    };

public func:
    explicit SMTPServer(unsigned bind_port=12005);
    ~SMTPServer();
    bool applySocket();


private mem : 
    unsigned connection_count = 0;
    thread _th;
    // bool acceptNewConnection = true;
    TCPSocket* welcome_socket;
    unordered_map<std::thread::id, SMTPClientState> thread_map;
    Command commands[7] = {
        {string("DATA"), std::bind(&SMTPServer::onDATA, this, std::placeholders::_1, std::placeholders::_2)},
        {string("HELO"), std::bind(&SMTPServer::onHELO, this, std::placeholders::_1, std::placeholders::_2)},
        {string("EHLO"), std::bind(&SMTPServer::onEHLO, this, std::placeholders::_1, std::placeholders::_2)},
        {string("MAIL"), std::bind(&SMTPServer::onMAIL, this, std::placeholders::_1, std::placeholders::_2)},
        {string("RCPT"), std::bind(&SMTPServer::onRCPT, this, std::placeholders::_1, std::placeholders::_2)},
        {string("RSET"), std::bind(&SMTPServer::onRSET, this, std::placeholders::_1, std::placeholders::_2)},
        {string("QUIT"), std::bind(&SMTPServer::onQUIT, this, std::placeholders::_1, std::placeholders::_2)},
    };

private func: 
    // bool relisten(){this->welcome_socket->listen();}
    void processSMTPequest(TCPSocket *);
    //smtp actions
    bool onGREET(TCPSocket *);
    bool onHELO(TCPSocket *, string);
    bool onEHLO(TCPSocket *, string);
    bool onMAIL(TCPSocket *, string);
    bool onRCPT(TCPSocket *, string);
    bool onDATA(TCPSocket *, string);
    bool onRSET(TCPSocket *, string);
    bool onQUIT(TCPSocket *, string);
    bool onWRONGCMD(TCPSocket *);
    bool onRAWDATA(TCPSocket *);
};
