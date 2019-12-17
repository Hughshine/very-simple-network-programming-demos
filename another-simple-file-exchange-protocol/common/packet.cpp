#include "packet.h"
using namespace std;
const string Packet::appid = "ANOTHER_FTP";

Packet::Packet(const Packet& packet)
{
    this->_appid = packet._appid;
    this->seq = packet.seq;
    this->is_ack = packet.is_ack;
    this->ackid = packet.ackid;
    this->is_psh = packet.is_psh;
    this->is_fin = packet.is_fin;
    this->ret_code = packet.ret_code;
    this->cmd_code = packet.cmd_code;
    this->username = packet.username;
    this->password = packet.password;
    this->authkey = packet.authkey;
    this->body = packet.body;
}

Packet::Packet(unsigned seq, bool is_ack, unsigned ackid, FtpRetCode ret_code, FtpCmdCode cmd_code, string body, string authkey, bool is_psh, bool is_fin, string username, string password)
    : _appid(appid), seq(seq % MAX_SEQ), is_ack(is_ack), ackid(ackid % MAX_SEQ), is_psh(is_psh), is_fin(is_fin), ret_code(ret_code), cmd_code(cmd_code), body(body), authkey(authkey), username(username), password(password)
{}

Packet::Packet(string packet)
{
    // cout << "[pkt]:" << packet << endl;
    auto components = get_header_and_body(packet);
    _appid = components[APPID];
    seq = (unsigned)atoi(components[SEQ].c_str());
    is_ack = (bool)atoi(components[ISACK].c_str());
    ackid = (unsigned)atoi(components[ACKID].c_str());
    is_psh = (bool)atoi(components[ISPSH].c_str());
    is_fin = (bool)atoi(components[ISFIN].c_str());
    ret_code = (FtpRetCode)atoi(components[RET_CODE].c_str());
    cmd_code = (FtpCmdCode)atoi(components[CMD_CODE].c_str());
    username = components[USERNAME];
    password = components[PASSWORD];
    authkey = components[AUTHKEY];
    body = components[BODY];
    if(body.length() > MAX_LENGTH)
    {
        cerr<<"[!!!body too large]"<<endl;
    }
}

Packet::Packet(FtpRetCode ret_code, FtpCmdCode cmd_code, string body, string authkey, bool is_psh, bool is_fin, string username, string password)
    :Packet(0, true, 0, ret_code, cmd_code, body, authkey, is_psh, is_fin, username, password)
{}

Packet::Packet()
    : Packet("0 0 0 0 0 0 0 0 0 0 0 0")
{}

Packet& Packet::operator=(const Packet& packet)
{
    this->_appid = packet._appid;
    this->seq = packet.seq;
    this->is_ack = packet.is_ack;
    this->ackid = packet.ackid;
    this->is_psh = packet.is_psh;
    this->is_fin = packet.is_fin;
    this->ret_code = packet.ret_code;
    this->cmd_code = packet.cmd_code;
    this->username = packet.username;
    this->password = packet.password;
    this->authkey = packet.authkey;
    this->body = packet.body;
    return *this;
}



string Packet::make()
{
    string packet = "";
    packet += appid; //
    packet += " ";
    packet += to_string(seq);
    packet += " ";
    packet += to_string((int)is_ack);
    packet += " ";
    packet += to_string(ackid);
    packet += " ";
    packet += to_string((int)is_psh);
    packet += " ";
    packet += to_string((int)is_fin);
    packet += " ";
    packet += to_string(ret_code);
    packet += " ";
    packet += to_string(cmd_code);
    packet += " ";
    packet += (username == "") ? "0" : username;
    packet += " ";
    packet += (password=="") ? "0" : password;
    packet += " ";
    packet += (authkey=="") ? "0" : authkey;
    packet += " ";
    packet += body;
    // cout << "[pkt]:" << packet << endl;
    return packet;
}

/**
 * 生成单纯的ack包，没有内容。
 */
Packet Packet::ack()
{
    Packet ack = Packet(ackid, true, seq+1, FTP_GOOD, FTP_NONE, "", authkey); // 就双方都用这个authkey交互吧，不论安全性。
    return ack;
}

vector<string> Packet::get_header_and_body(string packet)
{
    stringstream ss;
    ss.clear();
    ss.str(packet);
    vector<string> components;
    for(int i=0;i<11;i++)
    {
        string s;
        ss >> s;
        components.push_back(s);
    }
    ss.get();
    // cout << endl;
    // cout << "packet.len: " << packet.length() <<endl; 
    string body;
    std::getline(ss, body, (char)EOF); // TODO
    // cout << "body.len: " << body.length() << endl;
    components.push_back(body);
    ss.clear();
    return components;
}

void Packet::print()
{
    cout << "<SEQ>: " << seq << endl
         << "<is_ack>: " << is_ack << " <ackid>: " << ackid << endl
         << "<is_fin>: " << is_fin << endl
         << "<is_psh>: " << is_psh << endl
         << "<RET_CODE>:" << ret_code << endl
         << "<CMD_CODE>:" << cmd_code << endl
         << "<USERNAME>: " << username << endl
         << "<PASSWORD>: " << password << endl
         << "<AUTHKEY>: " << authkey << endl
         << "<Body>:" << endl
         << "\t$" << body << "$" << endl;
}