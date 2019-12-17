CC := g++
CFLAGS := -Wall -Wextra -std=c++11 -L/usr/local/Cellar/boost/1.71.0/lib -lboost_filesystem # no debug
TARGET := main

all: run

../common/pch.h.gch: ../common/pch.h
	g++ -x c++-header -o ../common/pch.h.gch -c ../common/pch.h

main: udpsocket ftpsocket clientconsole ../common/pch.h.gch
	$(CC) $(CFLAGS) $(shell ls *.cpp) $(shell ls ../common/*.cpp) -o main
	
.PHONY: all clean main run test udpsocket packet ftpsocket clientconsole common

run: main

common: ../common/common.h

packet: ../common/packet.h ../common/packet.cpp

udpsocket: ../common/udpsocket.h ../common/udpsocket.cpp

ftpsocket: ../common/ftpsocket.h ../common/ftpsocket.cpp

clientconsole: common ftpsocket clientconsole.h clientconsole.cpp 

clean:
	rm -f $(TARGET) *.o 
	rm -rf ./user/*/*

test: 
	./main

