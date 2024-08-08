from datetime import datetime
import threading

class BasicPrinter:
    #基本打字机类：此模块用于构建基本线程类，负责输出的控制

    def __init__(self):
        # 构造函数

        self.lock = threading.Lock()
        # 锁

    def __del__(self):
        # 析构函数
        pass
 
  
    def get_current_time_as_string():
        # 获取当前系统时间，并返回为字符串。
        current_time = datetime.now()  
        time_string = current_time.strftime("%Y-%m-%d %H:%M:%S")  
        return time_string  

    def print(self, name, string):
        # 打印一个字符串

        with self.lock:
            #进入临界区

            full_time_str = BasicPrinter.get_current_time_as_string()   
            # 获取当前时间  
            print("{:<24} {:<18} {:<24}".format(full_time_str, name, string))
            #打印字符串

    def justPrint(self, string):
        # 仅打印一个字符串

        with self.lock:
            #进入临界区
            
            print(string)
            #打印字符串
