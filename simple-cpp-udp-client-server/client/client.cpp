#include "client.h"
client::client(string target_ip, unsigned target_port){
	set(target_ip, target_port);
	// cout<<"Client is configured.\n";
}

int client::createSocket(){
	// Create socket at a random port.
	this->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 0) { error("socket()"); }
	return 0;
}


int client::sendCmd(string cmd=""){
	createSocket();
	sendto(s, cmd.c_str(), strlen(cmd.c_str())+1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
	socklen_t len;
	recvfrom(s, buffer, 512, 0, (struct sockaddr *)&server_addr, &len);
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