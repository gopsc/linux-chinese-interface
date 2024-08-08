import time
import psutil  

from qingth.BasicPrinter import BasicPrinter
from qingth.BasicThread import BasicThread

class CpuManager(BasicThread):
    #该对象用于周期性地获取处理器的使用率

    def __init__(self, name, script):
        #构造函数

        BasicThread.__init__(self, name, script)
        #调用父类的构造函数

    def __del__(self):
        #析构函数
        BasicThread.__del__(self)

    #------------------------------
    def _stop(self):
        #静止事件
        #self.event.wait()
        time.sleep(0.01)

    def _setup(self):
        #唤醒事件
        
        self['value'] = -1
        #设置变量池的变量
        self['status'] = 'running'
        # 设置局部脚本的状态

    def _loop(self):
        #循环体

        cpu_percent = psutil.cpu_percent(interval=0.1)  
        # 获取CPU使用率，interval=1表示psutil会等待0.1秒后计算CPU使用率 
        
        self['value'] = cpu_percent
        #将数据上传到脚本

        # 由于psutil.cpu_percent已经包含了0.1秒的等待时间，所以这里不需要额外的sleep

    def _clean(self):
        #清理事件

        try:
            del self['value']
            # 删除变量池中的变量
        except KeyError as e:
            pass
    #------------------------------