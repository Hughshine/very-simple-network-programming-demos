from socket import *


class UDPClient:
    def __init__(self, serverName = 'hostname', serverPort = 12000):
        self.serverName = serverName
        self.serverPort = serverPort
        # AF_INET 暗示使用ipv4。 SOCT_DGRAM 意味着是UDP的socket
        self.clientSocket = socket(AF_INET, SOCK_DGRAM)

    def sendAndRsv(self):
        message = input('input lowercase sentence: ')
        self.clientSocket.sendto(bytes(message, encoding='utf8'), (self.serverName, self.serverPort))
        modifiedMessage, serverAddress = self.clientSocket.recvfrom(2048)
        print('upper case: %s' % str(modifiedMessage, encoding='utf-8'))
        self.clientSocket.close()


if __name__ == '__main__':
    client = UDPClient('localhost', 12000)
    client.sendAndRsv()
