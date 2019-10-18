
# 作业一

client-server. server接受client 的command，Date 或 Time,
返回客户端日期（年月日）或（时分秒）。

## usage

for mac

```sh
sudo ./recompile.sh  # 修改代码后，重新compile
sudo ./start.sh  # 在两个新的终端分别运行client & server。
###### 或者
cd ./client  # or ./server
./compile.sh  # 重新compile client.out 或 server.out 并运行
./client.out # 直接运行
```

## 参考

 * [socket 编程](https://blog.csdn.net/u011467458/article/details/52585457)

 * [get time or date](http://en.cppreference.com/w/cpp/chrono/c/strftime)

 * 一张socket的函数调用图

   > client随机选一个端口就好，只有server需要有一个确定的端口，并且保持被动地监听。

![Client Server Model](./tcp.gif)

## TODO

- [x] 封装

- [x] 拓展client，可以选择ip，port

- [x] 拓展server，可以选择bind的port

- [x] simple cli.

- [ ] 没有封装exception，没有输入参数检查，没有文件存取，生硬的程序结束方式。

## 遇到的问题

1. `write(s, cmd.c_str(), strlen(cmd.c_str())+1);`时，要额外write一个空位置。

2. shell，要加sudo。
【被次级引用的shell，报错信息会被吞掉；访问网络，覆盖编译的输出，都要sudo】

    > 诶，还是用make比较好。。

3. socket close之后，socket也要重新获取，然后重新握手。

4. 目前是单线程同步的server，要注意这一点。

5. [mac上，terminal中打开新的终端](https://www.zhihu.com/question/36743280)

6. 启用一个server后，并接受过一次client的信息后，ctl+c强制退掉，要重启多次server后，client才能再次访问它。**是bind的问题**。对应端口被占用，没有及时恢复。

    > 大致可以这样复现，具体原因还不清楚。感觉问题就是强制退出后，一段时间内，相同端口还是在被占用状态，没有办法立刻去用它。

	* `bind()`与`listen()`设置errno时的对应含义：

	```
	bind():
		EACCESS 地址空间受保护，用户不具有超级用户的权限。
		EADDRINUSE  (48) 给定的地址正被使用。
	listen():
		EADDRINUSE   另一个套接字已经绑定在相同的端口上。
		EBADF      参数sockfd不是有效的文件描述符。
		ENOTSOCK    参数sockfd不是套接字。
		EOPNOTSUPP   参数sockfd不是支持listen操作的套接字类型。
	```

7. （linux/mac下）server读取client的发起ip，port。

```
clnt_sock = accept(s, (struct sockaddr *) &clnt_addr, &clnt_addr_size);
char client_ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET,&(clnt_addr.sin_addr), client_ip, sizeof(client_ip)); // client_ip is char[]
cout<<"Client IP: "<< client_ip<<endl;
cout<<"Client port: "<< clnt_addr.sin_port<<endl;
```

## 还没有解决的问题

> 诶。。上面的错误让我用好久。。我好菜。。

