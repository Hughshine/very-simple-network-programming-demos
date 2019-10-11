from socket import *

'''
一个TCP连接的唯一标识是[server ip, server port, client ip, client port].
也就是说，在接收到一个端口发来的数据时，会在该端口，产生的连接中，查找到符合这个唯一标识的并传递信息到对应缓冲区。
即多个socket可以占据同一个端口。【socket并行】
【accept后产生的connectionSocket，和serverSocket在同一个端口，只不过client ip,port 不同】
【而一个端口可以认为就是一个应用程序啦】
【多路分解、至于是谁在这个应用程序中产生的若干端口进行分发，就是应用程序呗，目前认为是都封装到对应的库中了。】
'''
class TCPClient:

    def __init__(self, serverName="localhost", serverPort=13002):
        self.serverName = serverName
        self.serverPort = serverPort
        self.clientSocket = socket(AF_INET, SOCK_STREAM)

    def connect(self):  # 只发送一个报文
        self.clientSocket.connect((self.serverName, self.serverPort))  # 会有refuse. UDP直接就可以发出去啦
        sentence = input('lower case sentence: ')
        self.clientSocket.send(bytes(sentence, encoding='utf8'))
        modifiedSentence = self.clientSocket.recv(1024)
        print('From Server: ', str(modifiedSentence, encoding='utf-8'))
        '''
        可以复制一遍，但是server端需要相应编程，对多次发送进行处理
        （比如说至少不能读一次就主动关掉）
        '''
        # sentence = input('lower case sentence: ')
        # self.clientSocket.send(bytes(sentence, encoding='utf8'))
        # modifiedSentence = self.clientSocket.recv(1024)
        # print('From Server: ', str(modifiedSentence, encoding='utf-8'))
        self.clientSocket.close()

if __name__ == '__main__':
    client = TCPClient()
    client.connect()