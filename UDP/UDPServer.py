from socket import *

class UDPServer:
    i = 0
    def __init__(self, serverName='hostname', serverPort=12000):
        self.serverName = serverName
        self.serverSocket = socket(AF_INET, SOCK_DGRAM)
        self.serverSocket.bind(('', serverPort))
        print('The server is ready to receive.')
        self.startRsving()

    def startRsving(self):
        while True:
            self.i += 1
            print('waiting for %d msg...' % self.i)
            message, clientAddress = self.serverSocket.recvfrom(2048)
            modifiedMessage = message.upper()
            self.serverSocket.sendto(modifiedMessage, clientAddress)


if __name__ == '__main__':
    server = UDPServer(serverName='localhost', serverPort=12000)
    server.startRsving()