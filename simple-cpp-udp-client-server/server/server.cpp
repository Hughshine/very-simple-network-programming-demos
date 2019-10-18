#include "server.h"
// string ip;
// int bind_port;
// struct sockaddr_in server_addr;
// char* buffer;
// 	int s;//socket

server::server(string ip, unsigned bind_port){
	set(ip, bind_port);
}

int server::bindSocket(){
	this->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->s < 0) { error("socket()"); }
	cout<<"socket: "<<this->s<<endl;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	server_addr.sin_port = htons(bind_port);
	this->bind_success = !testBind(10);
	return bind_success;
};

int server::dealWithRequests()
{
	struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
     
    while (true) {
        recvfrom(s, read_buffer, 512, 0, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clnt_addr.sin_addr), client_ip, sizeof(client_ip));

        cout<<"Client IP: "<< client_ip<<endl;
        cout<<"Client port: "<< clnt_addr.sin_port<<endl;
        cout<<"Command from client: "<< read_buffer<<endl;
        cout<<"Request time: "<<currentDate()<<" "<<currentTime()<<endl;
        string ret = "";
        if(!strcmp(read_buffer, "Date")) {
            ret = currentDate();
        }else if(!strcmp(read_buffer, "Time")) {
            ret = currentTime();
        }else {
            ret = "Sorry, wrong command.";
        }
//向客户端发送数据
        const char* str = ret.c_str();
        cout<<"Callback: "<<str<<endl;
        cout<< "Sizeof(callback): "<<strlen(str)<<endl;
        sendto(s, str, strlen(str)+1, 0,(struct sockaddr*)&clnt_addr, clnt_addr_size);
        cout<<" ----- Query times: "<<++this->query_times<<"! Finished... ----- "<<endl;
    }

//关闭套接字
   close(s);
   cout<<" --- Bye --- \n";
   return 0;
}


void server::resetConfig(string ip, unsigned bind_port)
{
	set(ip, bind_port);
}

int server::testBind(int times=10){
	int bind_code = -1;
	for(int i=0;i<times ;i++) {
		cout<<" --------- Trying to bind socket, attempts: "<< i+1<<"/10. --------- "<<endl;
		bind_code = bind(s, (struct sockaddr *) &server_addr, sizeof(server_addr));
		if (bind_code == 0)  {
			cout<<" --------- Bind success!!! --------- "<<endl;
			return 0;
			break;
		} 
        cout<<"error code: "<<errno<<endl;  // 48就是already in use的意思
        sleep(1);
    }
    if (bind_code != 0)
    {
    	cout<<" --------- Bind failed... --------- "<<endl;
    	cout<<"bind code: "<<bind_code<<endl;
    	cout<<" --------- Bind failed... --------- "<<endl;
    	return 1;
    }
    return 1;
}

void server::set(string ip, unsigned bind_port)
{
	this->ip = ip;
	this->bind_port = bind_port;
	this->bind_success = 0;
	this->query_times = 0;
	// this->read_buffer[64];
}

const std::string server::currentDate()
{
	time_t now = time(0);
	struct tm  tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
	return buf;
}

const std::string server::currentTime()
{
	time_t now = time(0);
	struct tm  tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
	return buf;
}

void server::error(const char *m)
{
	perror(m);
	exit(0);
}

