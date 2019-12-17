'''
发送一个带文本与图片附件的email
'''
import smtplib
from email import encoders
from email.header import Header
from email.mime.base import MIMEBase
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.utils import parseaddr, formataddr

def _format_addr(s):
    name, addr = parseaddr(s)
    return formataddr((Header(name, 'utf-8').encode(), addr))

from_addr = "1015545250@qq.com" # input('From: ')
to_addr = "1015545250@qq.com"
smtp_server = "127.0.0.1"

msg = MIMEMultipart()
msg['From'] = _format_addr('1711352 <%s>' % from_addr)
msg['To'] = _format_addr('管理员 <%s>' % to_addr)
msg['Subject'] = Header('I\'m SMTP', 'utf-8').encode()

msg.attach(MIMEText('附件是可爱的辉夜，请查收！', 'plain', 'utf-8'))

with open('./text/test.txt', 'rb') as f:
    # 设置附件的MIME和文件名
    mime = MIMEBase('image', 'txt', filename='test.txt')
    # 附件的必要的头信息
    mime.add_header('Content-Disposition', 'attachment', filename='test.txt')
    mime.add_header('Content-ID', '<0>')
    mime.add_header('X-Attachment-Id', '0')
    # 读入内容并解码
    mime.set_payload(f.read())
    encoders.encode_base64(mime)
    msg.attach(mime)

with open('./images/kaguya.jpg', 'rb') as f:
    # 设置附件的MIME和文件名
    mime = MIMEBase('image', 'jpg', filename='kaguya.jpg')
    # 附件的必要的头信息
    mime.add_header('Content-Disposition', 'attachment', filename='kaguya.jpg')
    mime.add_header('Content-ID', '<0>')
    mime.add_header('X-Attachment-Id', '0')
    # 读入内容并解码
    mime.set_payload(f.read())
    encoders.encode_base64(mime)
    msg.attach(mime)

server = smtplib.SMTP(smtp_server, 12004) 
server.set_debuglevel(1)
# server.login(from_addr, password)
server.sendmail(from_addr, [to_addr], msg.as_string())  
server.quit()