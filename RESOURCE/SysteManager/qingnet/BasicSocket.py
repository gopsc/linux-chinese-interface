import socket 

class BasicSocket:
    # 基本的套接字类，管理套接字的创建与释放
    
    def __init__(self):
        self.sock = None

    def __del__(self):
        pass

    def create_socket(self):
        #构造函数创建套接字
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
        # 创建一个IPv4, TCP的套接字  

    def delete_socket(self):
        #析构函数释放套接字
        self.sock.close()

    def bind_socket(self, addr, port):
        # 绑定套接字
        self.sock.bind((addr, port))