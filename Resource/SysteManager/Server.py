import socket 
import selectors  
import time  

from qingth.BasicThread import BasicThread
from qingnet.BasicSocket import BasicSocket
from Communication import Communication

class Server(BasicSocket, BasicThread):
    #服务器类，用于监听并与客户端进行通信
    def __init__(self, name, script):
        #构造函数
        BasicSocket.__init__(self)
        BasicThread.__init__(self, name, script)
        #在构造函数中调用父类的构造
    
    def __del__(self):
        #析构函数

        #...

        BasicThread.__del__(self)
        BasicSocket.__del__(self)
        #在析构函数中调用父类的析构

    #------------------------------

    def _stop(self):
        #静止事件
        #self.event.wait()
        time.sleep(0.01)

    def _setup(self):
        #唤醒事件

        self['printer'] = self.script['printer']
        #因为这个线程本身也是个母线程，所以它也需要printer变量

        addr = self.script['Configure']['Listen Address']
        port = int(self.script['Configure']['Listen Port'])
        maxnum = int(self.script['Configure']['Max Connections'])
        #获取参数

        BasicSocket.create_socket(self)
        # 创建一个IPv4, TCP的套接字  

        BasicSocket.bind_socket(self,addr,port)
        # 绑定到一个地址和端口 

        # 创建一个选择器并注册服务器套接字，关注读事件  
        self.sel = selectors.DefaultSelector()  
        self.sel.register(self.sock, selectors.EVENT_READ) 
   
        self.sock.listen(maxnum)
        # 开始监听连接

        self.script['printer'].print(self.name, 'Listening...') 

        self['count'] = 0
        self['status'] = 'running'

    def _loop(self):
        #循环体

        # 设置超时为1秒  
        events = self.sel.select(timeout=1)  
        for key, _ in events:  
            if key.data is None:  # 表示这是服务器套接字  

                client_socket, client_address = self.sock.accept()
                # 接受一个客户端连接

                cnt = 'Communication' + str(self.dictionary['count'])
                Communication(client_socket,client_address,cnt,self)
                # 创建一个Communication对象，并注册到选择器中

                self[cnt]['status'] = 'start'
                self['count'] += 1
                # 更新计数器

            else:  
                # 超时或其他非读事件，可以在这里处理超时逻辑  
                #print('等待连接超时...') 

                pass
                
    
    # 注意：在实际应用中，你应该在合适的位置关闭服务器套接字和选择器。  
    # 例如，你可以捕获KeyboardInterrupt异常来优雅地关闭它们。

    def _clean(self):
        #清理事件

        BasicSocket.delete_socket(self)
        #删除套接字

        try:
            #如何关闭所有线程？

            del self['count']
            #清理变量池
            
        except KeyError as e:
            pass
