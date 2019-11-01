'''
解码附件

TODO 可视化: https://zhuanlan.zhihu.com/p/22619896
'''
import os
import email
from email import policy
from email.header import Header, decode_header
from email.parser import BytesParser
import glob

def decode_eml(dir, filename):  # './xxx/'
    print('-------------------------------------------------------------------')
    print ('Decoding: ' + dir + filename + "\n")
    # with open(dir + filename, 'r') as fp:
    fp = open(dir + filename, 'rb')  # b => bytes
    msg = BytesParser(policy=policy.default).parse(fp)
    _from = msg.get('From')
    _to = msg.get('To')
    _subject = msg.get('Subject')
    print('From: ' + _from)
    print('To: ' + _to)
    print('Subject: ' + _subject + '\n')

    fp = open(dir + filename, 'r')
    msg = email.message_from_file(fp)
    for par in msg.walk():  # 对于每一个MIME块 
        if not par.is_multipart():     
            content_type = par.get('Content-Type')
            print('content_type: ' + content_type)
            name = par.get_param('filename')
            if name:
                h = Header(name)  # 解码奇怪的文件名
                dh = decode_header(h)
                fname = dh[0][0]  # 附件名
                print('附件:', str(fname, encoding='utf-8') + '\n')
                data = par.get_payload(decode=True)
                try:
                    f = open(dir + str(fname, encoding='utf-8'), 'wb')  # 注意一定要用wb来打开文件，因为附件一般都是二进制文件
                    f.write(data)
                    f.close()
                except:
                    print('error: 附件名含非法字符，存为tmp')
                    f = open('tmp', 'wb')
                    f.write(data)
                    f.close()
            else:
                print('文本内容: ', str(par.get_payload(decode=True), encoding='utf-8') + '\n')

    fp.close()
    print('--------------------------------End--------------------------------\n')

def decode_all(path):
    for (dirpath, dirnames, filenames) in os.walk(path):
        for filename in filenames:
            if filename.endswith('.eml'): 
                decode_eml(dirpath+"/", filename)
                
if __name__ == "__main__":
    decode_all('./mails/')
    


        