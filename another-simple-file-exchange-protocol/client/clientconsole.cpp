#include "clientconsole.h"
#define CommandType ClientConsole::CommandType

ClientConsole::ClientConsole()
    : isConnected(false), username("test"), password("password"), client(nullptr)
{

}

void ClientConsole::start()
{
    giveInfo("Welcome to another-simple-ftp-client.");
    while(true)
    {
        string full_cmd;
        cout << " > ";
        getline(cin, full_cmd);
        int code = handleCommand(full_cmd);
        if (code == COMMAND_QUIT)
        {
            giveInfo("Bye.");
            break;
        }
        else if (code == COMMAND_DISCONNECT)
        {
            this->isConnected = false;
            username = "";
            password = "";
        }
        else if (code == COMMAND_INVALID)
        {
            giveInfo("Invalid cmd, key in `h` for help");
        }
    }
}

void ClientConsole::giveInfo(string info)
{
    cout << " < " << info << endl;
}

ClientConsole::CommandType ClientConsole::handleCommand(string full_cmd)
{
    vector<string> tokens;
    boost::split(tokens, full_cmd, boost::is_any_of(" \t"), boost::token_compress_on);
    // 最后一个token可能为空
    if (tokens.back() == "" || tokens.back() == "\t")
        tokens.pop_back();
    string basic_cmd = tokens[0];
    // for(auto s:tokens){
    //     cout << s << endl;
    // }
    if(!isConnected)
    {
        if (basic_cmd == "h" || basic_cmd == "help")
        {
            return onHelp();
        }
        else if (basic_cmd == "q" || basic_cmd == "quit")
        {
            return COMMAND_QUIT;
        }
        else if (basic_cmd != "cnct")
        {
            giveInfo("use \"cnct <ip> <port>\" to connect the server");
            return COMMAND_INVALID;
        }
        return onConnect(tokens);
    }
    
    if(basic_cmd == "h" || basic_cmd == "help")
    {
        if (tokens.size() > 1)
            cerr << "Redundant tokens are discarded." << endl;
        return onHelp();
    }
    else if (basic_cmd == "lls")
    {
        return onLLS(tokens);
    }
    else if (basic_cmd == "rls")
    {
        return onRLS(tokens);
    }
    else if (basic_cmd == "get")
    {
        return onGetFile(tokens);
    }
    else if (basic_cmd == "send")
    {
        return onSendFile(tokens);
    }
    else if (basic_cmd == "q" || basic_cmd == "quit")
    {  
        client->sendFin();
        return COMMAND_QUIT;
    }
    else if (basic_cmd == "dcnct")
    {
        return COMMAND_DISCONNECT;
    }
    else
    {
        return COMMAND_INVALID;
    }
    return COMMAND_OK;
}

bool check_ip(const char *ip)
{
    using namespace boost::xpressive;
    cregex reg_ip = cregex::compile("(25[0-4]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])[.](25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[.](25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[.](25[0-4]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[1-9])");
    return regex_match(ip, reg_ip);
}

ClientConsole::CommandType ClientConsole::onConnect(const vector<string> &tokens)
{
    if(tokens.size()!=3)
    {
        cout << tokens.size() << endl;
        return COMMAND_INVALID;
    }
    string ip = tokens[1];
    string port = tokens[2];

    if (!check_ip(ip.c_str()))
    {
        giveInfo("Invalid IP address.");
        return COMMAND_OK;
    }
    giveInfo("connecting to " + ip + ":" + port);
    cout << "Your account: ";
    cin >> username;
    cout << "Your password: ";
    cin >> password;
    cin.ignore(1024, '\n');  // 清空缓冲区的`\n`
    if(client != nullptr)
        delete (client);
    client = new FtpClient(username, password, ip, port);
    client->apply(); 
    FtpRetCode code = client->sendAuth();
    if(code == FTP_GOOD)
    {
        isConnected = true;
        giveInfo("`cnct` success!");
        string path = "./user/" + username + "/";
        if(!bf::is_directory(path))
        {
            bf::create_directory(path);
        }
        return COMMAND_OK;
    }
    else
    {
        printFtpRetInfo(code);
        giveInfo("`cnct` failed, please check ftp error info.");
        return COMMAND_OK;
    }
    return COMMAND_OK;
}

CommandType ClientConsole::onHelp()
{
    cout << "another-simple-ftp-protocol"
         << "[options]" << endl
         << "Options:" << endl
         << "\tcnct <ip> <port>  Connect to the server" << endl
         << "\tdcnct             Disconnect to the server" << endl
         << "\th | help          Print this help" << endl
         << "\tq | quit          Quit this program" << endl
         << "*** after cnct ***" << endl
         //  << "\tlcd <remote_path>           Change local dir" << endl
         //  << "\trcd <remote_path>           Change remote dir" << endl
         << "\tlls           ls local folder" << endl
         << "\trls           ls remote folder" << endl
         << "\tget <local_file_name> <remote_file_name>           get remote file" << endl
         << "\tsend <local_file_name> <remote_file_name>          send local file" << endl;
    return COMMAND_OK;
}

CommandType ClientConsole::onLLS(const vector<string> &) 
{
    vector<string> allFileNames;
    unsigned int totalCount = 0;
    bf::directory_iterator end_iter;
    string path = "./user/" + username + "/";
    cout << path << endl;
    for (bf::directory_iterator iter("./user/" + username + "/"); iter != end_iter; iter++)
    {
        totalCount++;;
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
        cout << "Empty content." << endl;
        return COMMAND_OK;
    }
    int count=0;
    for (auto iter: allFileNames)
    {
        cout << setw(10) << iter;
        count++;
        if (count % 3 == 0)
        {
            cout << endl;
        }
    }
    cout << endl;
    return COMMAND_OK;
}

CommandType ClientConsole::onRLS(const vector<string> &)
{
    FtpRetCode code = client->sendRls(cout);
    printFtpRetInfo(code);
    return COMMAND_OK;
}

CommandType ClientConsole::onGetFile(const vector<string> & tokens)
{
    if (tokens.size() < 3)
    {
        giveInfo("Not enough arguments.");
        return COMMAND_INVALID;
    }
    string lfile = tokens[1];
    string rfile = tokens[2];

    string plfile = "./user/" + username + "/" + lfile;
    // 1. already exists locally(same target_filename)? If yes, cover it.
    if(bf::exists(plfile))
    {
        cout << "File already exists, covering" << endl;
    }
    FtpRetCode code = client->getFile(plfile, rfile);
    printFtpRetInfo(code);
    return COMMAND_OK;
}

CommandType ClientConsole::onSendFile(const vector<string> & tokens)
{
    if(tokens.size() < 3) {
        giveInfo("Not enough arguments.");
        return COMMAND_INVALID;
    }
    // 0. check if the file path is safe (out of the dic of the user's root)
    string lfile = tokens[1];
    string rfile = tokens[2];
    string plfile = "./user/" + username + "/" + lfile;
    if (!bf::exists(plfile))
    {
        cout << "Local file inexists, aborting" << endl;
        return COMMAND_OK;
    }
    FtpRetCode code = client->sendFile(plfile, rfile);
    printFtpRetInfo(code);
    return COMMAND_OK;
}

