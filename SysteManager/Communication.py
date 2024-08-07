import time
import socket

from qingth.BasicPrinter import BasicPrinter
from qingth.BasicThread import BasicThread

from qingnet.BasicSocket import BasicSocket

class Communication(BasicThread):
    #交流类，由服务器创建。输入一个客户端套接字

    def __init__(self, client_socket, client_address, name, script):
        #构造函数
        BasicThread.__init__(self, name, script)
        #在构造函数中调用父类的构造

        self.sock = client_socket
        #设置成员变量

        self.dictionary['status'] = 'stop'
        self.dictionary['Address'] = client_address[0]
        self.dictionary['Port'] = client_address[1]
        #设置字典域

    def __del__(self):
        #析构函数
        #...
        #self.sock.close()

        BasicThread.__del__(self)
        #在析构函数中调用父类的析构

    def _stop(self):
        #线程静止
        #self.event.wait()
        time.sleep(0.01)

    def _setup(self):
        #线程被唤醒
        self.sock.settimeout(1)  
        #设置超时为1秒
        self.script['printer'].print(self.name, 'A new connection from {}:{}'.format(self.dictionary['Address'],self.dictionary['Port']))    
        self.dictionary['status'] = 'running'

    def _loop(self):
        #循环体
        try:  
            # 接收数据并回应  
            while True:  
                data = self.sock.recv(1024)
                if not data:  
                    break  
                #print(f'接收到来自 {client_address} 的数据: {data.decode()}')  
                self.sock.sendall(data)  # 将接收到的数据原样发送回去 
                  
        except socket.timeout:  
            self.script['printer'].print(self.name, "Reciving timeout.")  
        finally:  
            self.dictionary['status'] = 'shutdown'

    def _clean(self):
        #清理事件
        self.sock.close()