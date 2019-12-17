#include "../common/pch.h"
#include "../common/ftpsocket.h"
namespace bf = boost::filesystem;
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
    CommandType onConnect(const vector<string> &);// connect and auth
    CommandType onHelp();
    // 为简化，设定每个用户文件仅存于根目录
    CommandType onLLS(const vector<string> &); // local ls
    CommandType onRLS(const vector<string> &); // remote ls
    CommandType onGetFile(const vector<string> &);
    CommandType onSendFile(const vector<string> &);

    void giveInfo(string);

    // string target_ip;
    // unsigned target_port;
};

bool check_ip(const char *ip);