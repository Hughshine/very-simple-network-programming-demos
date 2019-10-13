#include "client.h"
client::client(string target_ip, unsigned target_port){
	set(target_ip, target_port);
	// cout<<"Client is configured.\n";
}

int client::createSocket(){
		// Create socket at a random port.
	this->s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s < 0) { error("socket()"); }
	return 0;
}


int client::sendCmd(string cmd=""){
	while(true) {
		try {
			createSocket();
			if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
				throw "Somthing wrong with your server... \n";
			}
			break;
		} catch(char const* s) {
			cout<<s<<"Retry? y/n:";
			string str = "";
			cin>>str;
			if (str == "y")
			{
				cout<<"Retrying...\n";
				continue;
			} else {
				cout<<"Exiting...\n";
				break;
			}
		}
	}
	write(s, cmd.c_str(), strlen(cmd.c_str())+1);
	recv(s, buffer, 512, 0);
	cout<<"Message form server: "<<buffer<<endl;
	close(s);
	return 0;
}

int client::reset(string target_ip, unsigned target_port){
	set(target_ip, target_port);
	// cout<<"Client is reconfigured.\n";
	return 0;
}

int client::set(string target_ip, unsigned target_port) {
	this->target_ip = target_ip;
	this->target_port = target_port;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(target_ip.c_str());
	server_addr.sin_port = htons(target_port);
	return 0;
}

void client::error(const char *m){
	perror(m);
}