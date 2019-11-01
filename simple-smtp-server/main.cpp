#include<iostream>
#include "smtpsocket.h"
using namespace std;

int main()
{
    unsigned bind_port = 12004;
    SMTPServer smpt_server(bind_port); // default: localhost
    smpt_server.applySocket();
}