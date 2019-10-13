#include <iostream>
#include <exception>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

class client
{
public:
	client(string target_ip, unsigned target_port);

	int createSocket();

	int sendCmd(string cmd);

	int reset(string target_ip, unsigned target_port);
private:
	int set(string target_ip, unsigned target_port);

	void error(const char *m);

	int s;

	string target_ip;

	unsigned target_port;

	struct sockaddr_in server_addr;

	char* buffer;
};