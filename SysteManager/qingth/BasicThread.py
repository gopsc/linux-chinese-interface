
import threading
import time

from .TheScript import TheScript
from .BasicPrinter import BasicPrinter

class BasicThread(TheScript):
    #用于创建可控线程的基本类

    # 守护线程意味着，当主线程结束时，该线程也会被自动关闭  
    # 如果不设置为守护线程，那么即使主线程执行完毕，这个线程也会继续执行，直到它自己的任务完成  

    def __init__(self, name, script):
        # 线程初始化，可以传入一个上级哈希域，这样的话，线程的运行就可以影响到别的

        TheScript.__init__(self, {'status': 'stop', })
        # 初始化父类，初始化这个子类的哈希域

        self.name = name
        self.script = script
        # 持有变量的设置，这里的哈希域位于上级

        self.thread = threading.Thread(target=BasicThread._main, args=(self,)) #设置线程
        self.thread.start()
        #设置、启动线程

        self.script[name] = self
        # 将线程加入上级哈希域

    def __del__(self):
        #销毁线程
        
        TheScript.__del__(self)
        # @qing-20240603：这个类暂无析构函数

    def _main(basicThread):
        '''
        线程的主函数
        @qing.20240610:线程状态和上级脚本状态的判断，影响着线程的运行。
        因为上层可能为进程，在这里进程状态只有running和shutdown两种状态
        所以，线程的运行状态受到上层脚本状态的调控时，且只受running和shutdown两种状态
        所以，要在停止之前将线程状态设置为对应的。以关闭当前线程下的子线程的运行
        '''
        basicThread.script['printer'].print(basicThread.name, 'Thread Start.')
        # 准备工作已经完成

        while basicThread['status'] != 'shutdown' and basicThread.script['status'] != 'shutdown':
            #进入回字形循环

            if basicThread['status'] == 'stop' and basicThread.script['status'] == 'running':
                # 首次进入静止事件
                basicThread.script['printer'].print(basicThread.name, 'Stay.')
            while basicThread['status'] == 'stop' and basicThread.script['status'] == 'running':
                #正式进入静止事件
                basicThread._stop()

            if basicThread['status'] == 'start' and basicThread.script['status'] == 'running':
                # 唤醒事件
                basicThread.script['printer'].print(basicThread.name, 'Waked.')
                basicThread._setup()

            if basicThread['status'] == 'running' and basicThread.script['status'] == 'running':
                # 首次进入运行态
                basicThread.script['printer'].print(basicThread.name, 'Running.')
            while basicThread['status'] == 'running' and basicThread.script['status'] == 'running':
                # 正式进入运行态
                basicThread._loop()

            basicThread.script['printer'].print(basicThread.name, 'Clear.')
            basicThread._clean()
            #清理事件


        if basicThread.script['status'] == 'shutdown':
            #如果是因为上级脚本而关闭退出，其下所有的子线程也会被关闭
            basicThread['status'] = 'shutdown'
 
        basicThread.script['printer'].print(basicThread.name, 'Thread Finished')
        #线程结束
        del basicThread['status']
        #删除状态
        del basicThread.script[basicThread.name]
        #删除线程

    def wait(self):
        #等待线程结束

        self.thread.join()
        #等待线程结束

    def waitStart(self):
        #等待线程开始

        if self['status'] == 'start':
            time.sleep(0.01)