#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>
#include <time.h>
using namespace std;

/**
 * synchronous server
 */
class server{
public:
	server(string ip, unsigned bind_port);
	int bindSocket();
	int listen();
	int dealWithRequests();
	void resetConfig(string ip, unsigned bind_port);
private:
	int testBind(int times);
	void set(string ip, unsigned bind_port);

	const std::string currentDate();
	const std::string currentTime();
	void error(const char *m);

	string ip;
	int bind_port;
	struct sockaddr_in server_addr;
	char read_buffer[64];
	int s;//socket
	unsigned query_times;

	int bind_success;
};