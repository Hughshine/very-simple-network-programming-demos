#include "ftpsocket.h"
int MAX_TIMEOUT = 60;
int NUMBER_CONNECTIONS = 10;
void printFtpInfo(ostream &out, string info)
{
    out << "[FTP]: " << info << endl;
}

void printFtpRetInfo(FtpRetCode code)
{
    switch (code)
    {
    case FTP_GOOD:
        printFtpInfo(cout, "Success!");
        break;
    case FTP_UNAUTH:
        printFtpInfo(cerr, "Auth failed.");
        break;
    case FTP_FILE_INEXIST:
        printFtpInfo(cerr, "Remote file inexists.");
        break;
    case FTP_FILE_COVERED:
        printFtpInfo(cerr, "Remote file covered.");
        break;
    case FTP_BAD_NETWORK:
        printFtpInfo(cerr, "Bad network.");
        break;
    case FTP_BAD_CMD:
        printFtpInfo(cerr, "Bad command, connect your PC assistant.");
        break;
    default:
        printFtpInfo(cerr, "Unknown error code.");
    }
}

std::string random_string(size_t length)
{
    auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

FtpServer::FtpClientState::FtpClientState(string username, string password, string client_ip, unsigned client_port)
    : username(username), password(password), client_ip(client_ip), client_port(client_port), last_sent_seq(0), last_sent_ackid(0), timeout_count(0), filename(""), cnctSocket(nullptr)
{
    authkey = random_string(12);
    // print();
}

void FtpServer::FtpClientState::print()
{
    cout << "FtpClientState : [username]:" + username + ", [password]:" + password + " [authkey]:" + authkey << endl
         << "\t[client_ip]: " + client_ip + " [client_port]: " + to_string(client_port) << endl
         << "\t[last_sent_seq]: " + to_string(last_sent_seq) << " [last_sent_ackid]: " + to_string(last_sent_ackid) << endl
         << "\t[timeout]: " + to_string(timeout_count) << endl;
}
/********************************************************
 **************************FtpServer**********************
 *********************************************************/
FtpServer::FtpServer(unsigned bind_port)
    : connection_count(0), cmdSocket("127.0.0.1", bind_port, UdpSocket::SERVER_SOCKET)
{
}

FtpServer::~FtpServer()
{
    cmdSocket.close();
}

bool FtpServer::lookup(const string &username, const string &password)
{
    string user_root = "../server/user/" + username + "/";
    if(!bf::is_directory(user_root))
    {
        return false;
    }
    string pwd_path = user_root + "userinfo/password.txt";
    ifstream fd;
    fd.open(pwd_path);
    string pwd;
    fd >> pwd;
    if(password != pwd)
    {
        return false;
    }
    return true;
}

int FtpServer::apply()
{
    // 两级线程：
    // 1. 为每个会话一个线程
    cmdSocket.bind();
    string spkt;
    while (this->connection_count <= NUMBER_CONNECTIONS)
    {
        if(!cmdSocket.recv(spkt))
        {
            // printFtpInfo(cerr, "test null pkt"); // 超时。 TODO: cmdSocket 不应设超时时间。
            continue;
        }  
        Packet pkt = Packet(spkt);
        cout << "get one pkt!" << endl;
        // pkt.print();
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
        // processFtpRequest(state); // 暂时是单线程的
        thread *_th = new std::thread(&FtpServer::processFtpRequest, this, state);
        this->connection_count++;
        // 1.1 查看这个包我们要的包，并且有用户密码项，提取出来，创建一个新的FtpClientState, new thread process它，在一个新端口创建一个udpSocket，返回一个带authkey的包
        // connection 计数+1
        // 注意client之后要给新的这个socket所在端口发包
        // 1.2 别的包都丢掉，这个socket只接受 建立连接的包（连接由authkey维护）
    }
    // 2. 每个会话中，为每个下载动作开启一个线程
    return 0;
}

bool FtpServer::validate(string authkey, Packet pkt)
{
    if(pkt._appid == Packet::appid && pkt.authkey == authkey)
        return true;
    return false;
}

bool FtpServer::sendPureAckPacket(FtpServer::FtpClientState* state)
{
    Packet pkt = Packet(FTP_GOOD, FTP_NONE, "", state->authkey);
    pkt.seq = ++state->last_sent_seq;
    pkt.is_ack = true;
    pkt.ackid = ++state->last_sent_ackid;
    state->last_sent_pkt = pkt;
    state->cnctSocket->send(pkt.make());
    return true;
}

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
 * 得到期待的包。此函数会做重传处理、有效性检查等操作。
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

/**
 * start a new thread with a udpsocket
 */
void FtpServer::processFtpRequest(FtpServer::FtpClientState *state)
{
    // 0. get a new udpsocket, onAuth(), return a packet with retCode + authkey + ack
    Packet success_pkt = Packet(0, true, 1, FTP_GOOD, FTP_NONE, "", state->authkey);
    state->last_sent_seq++;
    state->last_sent_ackid++; // 初始是0
    UdpSocket* cnctSocket = new UdpSocket(state->client_ip, state->client_port, UdpSocket::CLIENT_SOCKET); // 自己随机选一个可用接口与此客户通信
    state->cnctSocket = cnctSocket;
    cnctSocket->send(success_pkt.make());
    state->print();
    state->last_sent_pkt = success_pkt;
    Packet rpkt;
    while(true)
    {
        state->print();
        if (!this->recvValidPacket(state, rpkt))
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
        switch(rpkt.cmd_code) // TODO: here
        {
            case FTP_LIST:
                cout << "cmd: ls" << endl;
                ret = onListDicContent(state);
                // cout << "cmd: rls, success" << endl;
                break;
            case FTP_SEND_TO_SERVER:
                state->filename = rpkt.body;
                cout << "cmd: send" << endl;
                ret = onRecvFileFromClient(state);
                break;
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

/**
 * ls
 */
int FtpServer::onListDicContent(FtpServer::FtpClientState *state)
{
    vector<string> allFileNames;
    unsigned int totalCount = 0;
    bf::directory_iterator end_iter;
    string path = "./user/" + state->username + "/";
    cout << path << endl;
    for (bf::directory_iterator iter("./user/" + state->username + "/"); iter != end_iter; iter++)
    {
        totalCount++;
        ;
        string filename = iter->path().string();
        filename = filename.substr(path.length());
        if (!bf::is_directory(*iter))
        {
            totalCount++;
            allFileNames.push_back(filename);
        }
    }
    if (allFileNames.empty())
    {
        allFileNames.push_back("<empty>");
    }
    int count = 0;
    string data;
    for (auto iter : allFileNames)
    {
        data += iter + "\t";
        count++;
        if (count % 5 == 0)
        {
            data += "\n";
        }
    }
    // if(data.length() <= 4096)
    // {
    //     Packet pkt = Packet(FTP_GOOD, FTP_NONE, data, state->authkey, true);
    //     this->sendValidPacket(state, pkt);
    //     Packet rpkt;
    //     if(!this->recvValidPacket(state, rpkt)) // 第一次完成了，第二次调用状态不对
    //     {
    //         cerr << "cmd:[list] timeout" << endl;
    //         return CMD_DISCONNECT; // 连接中断
    //     }
    //     return CMD_GOOD;
    // }
    // else
    // {
        int fcount = data.length() / 512 + 1;
        int i;
        for(i=0;i<fcount-1;i++)
        {
            Packet pkt = Packet(FTP_GOOD, FTP_NONE, data.substr(512 * i, 512), state->authkey, false);
            this->sendValidPacket(state, pkt);
            Packet rpkt;
            if (!this->recvValidPacket(state, rpkt)) // 第一次完成了，第二次调用状态不对
            {
                cerr << "cmd:[list] timeout" << endl;
                return CMD_DISCONNECT; // 连接中断
            }
        }
        Packet pkt = Packet(FTP_GOOD, FTP_NONE, data.substr(512 * i, 512), state->authkey, true);
        this->sendValidPacket(state, pkt);
        Packet rpkt;
        if (!this->recvValidPacket(state, rpkt)) // 第一次完成了，第二次调用状态不对
        {
            cerr << "cmd:[list] timeout" << endl;
            return CMD_DISCONNECT; // 连接中断
        }
        return CMD_GOOD;
    // }
}

int FtpServer::onRecvFileFromClient(FtpServer::FtpClientState *state)
{
    ifstream ifile;
    ifile.open(""); // 先不处理不存在的情况

    return CMD_GOOD;
}

int FtpServer::onSendFileToClient(FtpServer::FtpClientState *state)
{
    ifstream ifile;
    ifile.open("./user/"+ state->username +"/" + state->filename); // 先不处理不存在的情况
    cout << string("!!opening...") + "./user/" + state->username + "/" + state->filename;
    if (!ifile.is_open())
    {
        Packet pkt = Packet(FTP_FILE_INEXIST, FTP_NONE, "", state->authkey);
        this->sendValidPacket(state, pkt);
        return CMD_GOOD;
    }

    char buffer[1024];
    int i=0;
    while(!ifile.eof())
    {
        i++;
        ifile.read(buffer, 511);
        // cout << "sending length: " << string(buffer).length() <<endl;
        Packet pkt = Packet(FTP_GOOD, FTP_NONE, string(buffer), state->authkey, false);
        this->sendValidPacket(state, pkt);
        Packet rpkt;
        if (!this->recvValidPacket(state, rpkt))
        {
            cerr << "cmd:[list] timeout" << endl;
            return CMD_DISCONNECT; // 连接中断
        }
        memset(buffer, 0, 1024);
    }
    Packet pkt = Packet(FTP_GOOD, FTP_NONE, "", state->authkey, true);
    this->sendValidPacket(state, pkt);
    Packet rpkt;
    if (!this->recvValidPacket(state, rpkt)) // 第一次完成了，第二次调用状态不对
    {
        cerr << "cmd:[list] timeout" << endl;
        return CMD_DISCONNECT; // 连接中断
    }
    cout << "send file: " + state->filename + " ok!" << endl;
    cout << "packet count: " << ++i << endl;
    ifile.close();
    return CMD_GOOD;
}

// 返回给client错误信息，结束连接
int FtpServer::onWrongCmd(FtpServer::FtpClientState *state)
{
    return CMD_GOOD;
}

int FtpServer::onSendRaw(FtpServer::FtpClientState *state)
{
    return CMD_GOOD;
}

int FtpServer::onRecvRaw(FtpServer::FtpClientState *state)
{
    return CMD_GOOD;
}

int FtpServer::onSayBye(FtpServer::FtpClientState *state)
{

    return CMD_GOOD;
}


/********************************************************
 **************************Ftpclient**********************
 *********************************************************/
FtpClient::FtpClient(string username, string password, string ip, string port)
    :username(username), password(password), server_ip(ip), server_port(port)
{
}

FtpClient::~FtpClient()
{
    cmdSocket->close();
}

/**
 * apply (udp)socket on a random valid port
 */
int FtpClient::apply()
{
    cmdSocket = new UdpSocket(server_ip, atoi(server_port.c_str()), UdpSocket::CLIENT_SOCKET);
    return 0;
}

FtpRetCode FtpClient::sendAuth()
{
    Packet pkt = Packet(0, true, 0, FTP_GOOD, FTP_NONE, "", "0", false, false, username, password);
    cmdSocket->send(pkt.make());
    string ret;
    while(true)
    {
        // cout << "hello, I'm sending auth" << endl;
        if(!cmdSocket->recv(ret))
        {
            this->timeout_count++;
            cmdSocket->send(pkt.make());
            if(this->timeout_count >= 100){
                this->timeout_count = 0;
                return FTP_BAD_NETWORK;
            }
            continue;
        }
        last_sent_seq++, last_sent_ackid++;
        this->timeout_count = 0;
        Packet rpkt = Packet(ret);
        this->authkey = rpkt.authkey;
        cout << "authkey: " << authkey << endl;
        return (FtpRetCode)rpkt.ret_code;
    }
}

FtpRetCode FtpClient::sendRls(ostream &out)
{
    Packet pkt = Packet(FTP_GOOD, FTP_LIST, "", this->authkey);
    this->sendValidPacket(pkt);
    Packet rpkt;
    while(true)
    {
        if(!this->recvValidPacket(rpkt))
        {
            return FTP_BAD_NETWORK;
        }
        // TODO: send a pure ack;
        this->sendPureAckPacket(); // ackid 不增加
        out << rpkt.body;
        if(rpkt.is_psh)
        {
            out << endl;
            break;
        }
    }
    return FTP_GOOD;
}

FtpRetCode FtpClient::sendFile(string lfile, string rfile)
{
    return FTP_GOOD;
}

FtpRetCode FtpClient::getFile(string lfile, string rfile)
{
    Packet pkt = Packet(FTP_GOOD, FTP_GET_FROM_SERVER, rfile, this->authkey);
    this->sendValidPacket(pkt);
    Packet rpkt;
    ofstream fout;
    int i=0;
    fout.open(lfile);
    while (true)
    {
        if (!this->recvValidPacket(rpkt))
        {
            fout.close();
            cout << "保存失败" << endl;
            return FTP_BAD_NETWORK;
        }
        if(rpkt.ret_code == FTP_FILE_INEXIST)
        {
            return FTP_FILE_INEXIST;
        }
        if (rpkt.is_psh)
        {
            i++;
            cout << "recved pkt: " << i << endl;
            this->sendPureAckPacket(); // ackid 不增加？
            break;
        }
        this->sendPureAckPacket(); // ackid 不增加？
        i++;
        // cout << "body length: " << rpkt.body.length() << endl;
        fout << rpkt.body;
    }
    fout.close();
    return FTP_GOOD;
}

FtpRetCode FtpClient::sendFin()
{
    Packet pkt = Packet(FTP_GOOD, FTP_NONE, "", this->authkey, false, true);
    this->sendValidPacket(pkt);
    return FTP_GOOD;
}
int FtpClient::onSendRaw()
{
    return 0;
}

int FtpClient::onRecvRaw()
{
    return 0;
}



bool FtpClient::validate(string authkey, Packet pkt)
{
    if (pkt._appid == Packet::appid && pkt.authkey == authkey)
        return true;
    return false;
}

bool FtpClient::sendPureAckPacket()
{
    Packet pkt = Packet(FTP_GOOD, FTP_NONE, "", this->authkey);
    pkt.seq = ++last_sent_seq;
    pkt.is_ack = true;
    pkt.ackid = ++last_sent_ackid;
    last_sent_pkt = pkt;
    cmdSocket->send(pkt.make());
    return true;
}

bool FtpClient::sendValidPacket(Packet &pkt)
{
    pkt.seq = ++last_sent_seq;
    pkt.is_ack = true;
    pkt.ackid = ++last_sent_ackid;
    last_sent_pkt = pkt;
    cmdSocket->send(pkt.make());
    return true;
}

bool FtpClient::recvValidPacket(Packet &pkt)
{
    string ret;
    while (true)
    {
        if (!cmdSocket->recv(ret))
        {
            timeout_count++;
            if (timeout_count >= MAX_TIMEOUT)
            {
                cout << "recvValidPacket() timeout." << endl;
                return false;
            }
            continue;
        }
        Packet rpkt = Packet(ret);
        if (!this->validate(authkey, rpkt))
        {
            cout << "!!!!收到无效包" << endl;
            continue;
        }
        // 不是下一个包，重传
        if (rpkt.ackid < last_sent_seq // 两者应是等价的
            || rpkt.seq < last_sent_ackid)
        {
            cout << "!!!![重传]" << endl;
            cmdSocket->send(last_sent_pkt.make());
            continue;
        }
        last_sent_seq = rpkt.ackid-1;
        last_sent_ackid = rpkt.seq;
        pkt = rpkt;
        return true;
    }
    return true;
}

/**
 * stdout错误信息，结束连接. useless
 */
int FtpClient::onWrongCmd()
{
    return 0;
}