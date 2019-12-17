#include "smtpsocket.h"

SMTPServer::SMTPServer(unsigned bind_port)
{
    this->welcome_socket = new TCPSocket(bind_port);
    int bind_code = this->welcome_socket->bind(); // TODO
    if (bind_code != STATUS_OK) {
        cerr << "TCPSocket failed to bind." << endl;
    }
}

SMTPServer::~SMTPServer(){}

bool SMTPServer::applySocket()
{
    int listen_code = this->welcome_socket->listen();
    if(listen_code == STATUS_FAIL){
        cout << "TERMINATING!" <<endl;
        return false;
    }

    while(this->connection_count <= NUMBER_CONNECTIONS){
        TCPSocket* connect_socket = this->welcome_socket->accept();
        // TODO: uncontrolled multithread, 应控制数量与回收，线程标记。
        // 其实没有错，vscode插件错误报错
        thread* _th = new std::thread(&SMTPServer::processSMTPequest, this, connect_socket);
    }
    return true;
}

void SMTPServer::processSMTPequest(TCPSocket * connect_socket)
{
    auto thread_id = std::this_thread::get_id();
    cout << "On thread[" << thread_id << "]: "
         << "New SMTP transaction started"<< endl;
    this->onGREET(connect_socket);
    while(true) { // maybe set a timer
        string cmd = connect_socket->rcv();
        if (cmd == "")
        {
            cout << "thread" << thread_id << " closed\n";
            return;
        }
        findAndReplaceAll(cmd, "\n", "");
        findAndReplaceAll(cmd, "\r", "");
        cout << "On thread[" << thread_id << "]: "<< "Command from client <" << cmd << ">" << endl;
        bool GOOD_CMD = false;
        for(auto c:commands){
            // cout << c.command << endl;
            if(to_upper(cmd.substr(0,4))==c.command) {
                bool status = c.callback(connect_socket, cmd);
                if(status == STATUS_FINISHED) // QUIT, STATUS_FAIL ==> STATUS_FINISHED
                {
                    cout << "On thread[" << thread_id << "]: closed\n";
                    return;
                }
                else 
                {
                    GOOD_CMD = true;
                    break;
                }
            }
        }
        if(GOOD_CMD == false) {
            this->onWRONGCMD(connect_socket);
        }
    }
    cout << "On thread[" << thread_id << "]: closed\n";
    return;
}

bool SMTPServer::onGREET(TCPSocket *connect_socket)
{
    connect_socket->send("220 hello from smtp.hughshine.rip\r\n"); // do some checks？
    return STATUS_OK;
}
//smtp actions
bool SMTPServer::onHELO(TCPSocket * connect_socket, string)
{
    connect_socket->send("250 hello!!!\r\n");
    return STATUS_OK;
}
bool SMTPServer::onEHLO(TCPSocket * connect_socket, string)
{
    connect_socket->send("250 hello!!!\r\n");
    return STATUS_OK;
}
bool SMTPServer::onMAIL(TCPSocket * connect_socket, string cmd)
{
    string from = cmd.substr(5);
    auto thread_id = std::this_thread::get_id();
    if (this->thread_map.find(thread_id) == thread_map.end())
    {
        thread_map.insert(std::make_pair(thread_id, SMTPClientState()));
    }
    this->thread_map[thread_id].mailFrom = from;

    connect_socket->send("250 hello!!!\r\n");
    return STATUS_OK;
}
bool SMTPServer::onRCPT(TCPSocket *connect_socket, string cmd)
{
    string to = cmd.substr(5);
    auto thread_id = std::this_thread::get_id();
    if(this->thread_map.find(thread_id) == thread_map.end())
    {
        thread_map.insert(std::make_pair(thread_id, SMTPClientState()));
    }
    this->thread_map[thread_id].recpTo = to;

    connect_socket->send("250 hello!!!\r\n");
    return STATUS_OK;
}

bool SMTPServer::onDATA(TCPSocket * connect_socket, string)
{
    auto thread_id = std::this_thread::get_id();
    if (this->thread_map.find(thread_id) == thread_map.end())
    {
        connect_socket->send("500 Info Incomplete\r\n");
        return false;
    }    
    connect_socket->send("354 End data with <CR><LF>.<CR><LF>\r\n");
    this->onRAWDATA(connect_socket);
    connect_socket->send("250 OK!!!\r\n");
    return STATUS_OK;
}

bool SMTPServer::onRAWDATA(TCPSocket * connect_socket)
{
    string data = "";
    auto thread_id = std::this_thread::get_id();
    SMTPClientState client_state = this->thread_map[thread_id];
    string from = client_state.mailFrom;
    string to = client_state.recpTo;
    while(true)
    {
        string bytes = connect_socket->rcv();
        if (bytes == "")
        {
            cout << " On thread[" << thread_id <<"]: "<<"socket closed by client" << endl;
            return STATUS_OK;
        }

        if (hasEnding(bytes, ".\r\n")) //data 读取完毕，write to a file
        {
            string dir = "./mails/" + from + "/" + to + "/" + string(currentDate()) + "_" + string(currentTime()) + "/";
            boost::filesystem::create_directories(dir);
            findAndReplaceAll(bytes, "\r\n", "\n");
            data += bytes.substr(0, bytes.length()-3);
            ofstream file;
            string filename = dir + "email.eml";
            file.open(filename, ios::out | ios::trunc);
            file << data;
            file.close();
            cout << "On thread[" << thread_id << "]: "<< "email saved at \""<<filename<<"\" successfully." << endl;
            return STATUS_OK;
        }
        else
        {
            findAndReplaceAll(bytes, "\r\n", "\n");
            data += bytes;
        }
    }
}

bool SMTPServer::onRSET(TCPSocket * connect_socket, string)
{
    auto thread_id = std::this_thread::get_id();
    if (this->thread_map.find(thread_id) != thread_map.end())
    {
        thread_map.erase(thread_id);
    }
    connect_socket->send("220 reset\r\n");
    return STATUS_OK;
}
bool SMTPServer::onQUIT(TCPSocket * connect_socket, string)
{
    connect_socket->send("221 bye!!!\r\n");
    connect_socket->close();
    return STATUS_FINISHED;
}

bool SMTPServer::onWRONGCMD(TCPSocket * connect_socket)
{
    connect_socket->send("500 wrong command\r\n");
    return STATUS_FINISHED;
}
