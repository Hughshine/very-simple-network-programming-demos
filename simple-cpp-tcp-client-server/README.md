
# 作业一

client-server. server接受client 的command，Date 或 Time,
返回客户端日期（年月日）或（时分秒）。

## 参考
 * [socket 编程](https://blog.csdn.net/u011467458/article/details/52585457)

 * [get time or date](http://en.cppreference.com/w/cpp/chrono/c/strftime)

## TODO

- [ ] 封装

    > exception

- [ ] 拓展client，可以选择ip，port

- [ ] 拓展server，可以选择bind的port

- [ ] cli，控制server/client【可以不允许有多个client】的启停，配置改变，log查看等。

    > 可能需要查找进程。

## 遇到的问题

1. `write(s, cmd.c_str(), strlen(cmd.c_str())+1);`时，要额外write一个空位置。

2. shell，要加sudo。
【被次级引用的shell，报错信息会被吞掉；访问网络，覆盖编译的输出，都要sudo】

    > 诶，还是用make比较好。。

3. socket close之后，socket也要重新获取，然后重新握手。

4. 目前是单线程同步的server，要注意这一点。

## 还没有解决的问题

1. 启用一个server后，ctl+c强制退掉，要重启多次后client才能访问它。

    > 大致可以这样复现，具体原因还不清楚。