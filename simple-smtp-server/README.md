# 编写SMTP server并观察通信过程

## 基本要求

* 响应客户SMTP命令，显示交互过程和收到的邮件
* 支持单用户
* 不保存和转发收到的邮件
* 不做错误处理

邮件体编码：base64，支持图片传输

## usage

请定制化修改

```bash
# dependency: boost
make
make test
# another terminal
python send_mail.py
python decode_email.py
```

## TODO

- [x] TCPSocket完善一下。 10min
- [x] SmtpSocket方法除Data外封装。1h
- [x] Data命令 解码、发送 (没有做解码，不需要解的呀。。这部分是解码器做的事。)
- [x] multithread. 30min

- [x] TCPSocket 预期功能
- [x] Makefile
  
- [x] python写一个发邮件的脚本，不用软件或telnet
- [x] 分级的邮件存储
- [x] 更好的提示性信息
- [x] 更安全的multithread (不想写了)
- [x] 更好的邮件存储、解码

> - [ ] 简单的可视化。 
> > 不好意思，不想做了。

## References

SunriseFox学长的[very-simple-smtp-server](https://github.com/SunriseFox/very-simple-smtp-server)，架构（大概是这个词叭QAQ）思路真的是超级棒。在拙劣地模仿。

## MIME

Multipurpose Internet Mail Extensions 或 MIME

```txt
text/plain
text/html
image/jpeg
image/png
audio/mpeg
audio/ogg
audio/*
video/mp4
application/*
application/json
application/javascript
application/ecmascript
application/octet-stream
```

## in practice

TCP链接后，返回220
支持的命令：
HELO 
EHLO
HELP
DATA
MAIL FROM
RCPT TO (返回550表示email不存在)
RSET 重置内容
QUIT

## boost

[link boost in makefile](https://stackoverflow.com/questions/1305530/including-boost-libraries-in-make-files)

## others

vscode 真的好用

vscode 进行c_cpp配置： shift+cmd+p [edit configure]

返回250表示成功

nslookup, 查dns

tls 传输层安全协议

telnet ctrl+] quit 退出连接
