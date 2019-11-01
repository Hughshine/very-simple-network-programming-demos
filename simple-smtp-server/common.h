#ifndef COMMON_H
#define COMMON_H
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
#include <thread>
#include <list>
#include <functional>
#include <fstream>
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <ctime>
#include <time.h>
using namespace std;

string to_upper(string str);
bool hasEnding(string const &str, string const &end);
void findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr);
const std::string currentDate();
const std::string currentTime();

#endif