import time
import psutil

from qingth.BasicPrinter import BasicPrinter
from qingth.BasicThread import BasicThread

class MemoryManager(BasicThread):
    # 该对象周期性地获取内存的使用率

    def __init__(self, name, script):
        # 构造函数

        BasicThread.__init__(self, name, script)
        # 在构造函数中调用父类的构造

        # ...

    
    def __del__(self):
        #析构函数

        #...

        BasicThread.__del__(self)
        #在析构函数中调用父类的析构函数

    #------------------------------

    def _stop(self):
        #静止事件
        #self.event.wait()
        time.sleep(0.01)

    def _setup(self):
        #唤醒事件
        self['value'] = -1
        self['status'] = 'running'

    def _loop(self):
        #循环体

        mem_percent = psutil.virtual_memory().percent  
        # 获取内存使用率，这里我们只需要获取使用的百分比 

        self['value'] = mem_percent
        #将数据上传到脚本
        
        time.sleep(0.1)
        #休息一段时间

    
    def _clean(self):
        #清理事件
        
        try:
            del self['value']
        except KeyError as e:
            pass
    #------------------------------