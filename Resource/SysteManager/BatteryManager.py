import time
import psutil

from qingth.BasicPrinter import BasicPrinter
from qingth.BasicThread import BasicThread

class BetteryManager(BasicThread):
    # 该对象周期性地获取电池的使用率

    def __init__(self, name, script):
        #构造函数

        BasicThread.__init__(self, name, script)
        # 在构造函数中调用父类的构造

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
        # 将数组赋值给变量池
        self['status'] = 'running'
        #设置局部脚本


    def _loop(self):
        #循环体

        battery = psutil.sensors_battery()
        #获取电池信息

        if not battery:
            #没成功获取
            percent = 0
        else:
            #成功获取
            percent = battery.percent
            # 获取电池容量

        self['value'] = percent
        #将数据上传到脚本

        time.sleep(1)
        #休息一段时间

    
    def _clean(self):
        #清理事件

        try:
            del self['value']
            #删除变量池中的变量
        except KeyError as e:
            pass
        
