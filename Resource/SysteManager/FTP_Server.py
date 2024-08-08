
#pip install pyftpdlib

import threading 
from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer
import time

from qingth.BasicThread import BasicThread

class FTP_Server(BasicThread):
    # FTP服务器类

    def __init__(self, name, script):
        #构造函数
        BasicThread.__init__(self, name, script)
        #在构造函数中调用父类的构造

        #...

    def __del__(self):
        #析构函数

        #...

        BasicThread.__del__(self)
        #在析构函数中调用父类的析构

    #------------------------------

    def _stop(self):
        #静止事件
        #self.event.wait()
        time.sleep(0.01)

    def _setup(self):
        #唤醒事件
        path = self.script['Configure']['FTP Server Path']

        self.authorizer = DummyAuthorizer()
        #FTP服务器配置和启动代码
        self.authorizer.add_user("qing", "44112233", path, perm="elradfmw")
        #添加一个用户，参数分别为：用户名、密码、用户主目录、权限

        self.handler = FTPHandler
        self.handler.authorizer = self.authorizer

        addr = self.script['Configure']['Listen Address']
        server = FTPServer((addr, 21), self.handler)

        def monitor_sig_and_close_server():
            while self['status'] == 'running' and self.script['status'] == 'running':
                time.sleep(0.1) #每毫秒监察一次信号量
            self.script['printer'].print(self.name, 'Shutting down the FTPS Server...')
            server.close()
            self.script['printer'].print(self.name, 'Clear.')

        self['status'] = 'running'
        #线程开始监视

        monitor_thread = threading.Thread(target=monitor_sig_and_close_server)
        monitor_thread.start()
        # 启动一个线程来监控sig变量
        
        self.script['printer'].print(self.name, 'Listening...')
        #线程开始监听

        try:
            server.serve_forever()
            # 启动FTPS服务器
        except Exception as e:
            # 遇到任何问题

            self.script['printer'].print(self.name, 'unexpected Terminated.')
            #打印错误信息

            if self['status'] == 'running':
                #暂停运行
                self['status'] = 'stop'

            

    def _loop():
        #循环体
        time.sleep(0.1)

    
    def _clean(self):
        #清理事件

        try:

            del self.authorizer
            del self.handler
            #清理变量

        except AttributeError as e:
            pass
