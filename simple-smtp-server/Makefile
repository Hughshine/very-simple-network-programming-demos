CC := g++
CFLAGS := -Wall -Wextra -g -std=c++11 -L/usr/local/Cellar/boost/1.71.0/lib -lboost_filesystem
TARGET := main

all: run

main: smtpsocket
	$(CC) $(CFLAGS) $(shell ls *.cpp) -o main
	
.PHONY: all clean main run test tcpsocket smtpsocket base64 

run: main

tcpsocket: tcpsocket.h tcpsocket.cpp

smtpsocket: tcpsocket smtpsocket.h smtpsocket.cpp common.h common.cpp

clean:
	rm -f $(TARGET) *.o 
	rm -rf ./mails/*

test: 
	./main

