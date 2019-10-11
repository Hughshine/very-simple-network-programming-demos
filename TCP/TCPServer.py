from socket import *

class TCPServer:

    def __init__(self, serverPort=13002):
        self.serverPort = serverPort
        self.serverSocket = socket(AF_INET, SOCK_STREAM)


    def bind(self):
        self.serverSocket.bind(('', self.serverPort))
        self.serverSocket.listen(1)
        print('start receiving... :')
        while True:
            connectionSocket, addr = self.serverSocket.accept()
            sentence = connectionSocket.recv(2014)
            modified = sentence.upper()
            connectionSocket.send(modified)
            print(self.serverSocket)
            print(connectionSocket)
            # sentence = connectionSocket.recv(2014)
            # modified = sentence.upper()
            # connectionSocket.send(modified)
            connectionSocket.close()


if __name__ == '__main__':
    client = TCPServer()
    client.bind()


'''
fd => field descriptor.
每个链接都需要一个对应的描述，也就是惟一的ID，即对应的文件描述符.
'''