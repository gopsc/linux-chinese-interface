import sys

from qingth.TheScript import TheScript           # 线程组件，脚本域
from qingth.BasicPrinter import BasicPrinter     # 线程组件，打印域
from myjson.JsonOperation import JsonOperation   # json操作
from command import commands                     # 命令

from CpuManager import CpuManager                # cpu管理
from MemoryManager import MemoryManager          # 内存管理
from BatteryManager import BetteryManager        # 电池管理
from Server import Server                        # 服务器
from FTP_Server import FTP_Server                # FTP服务器

length = len(sys.argv)
if not (length == 1 or length == 2 or length == 3):
    raise Exception('You can input Program Mode & Configure JSON File Path.')
    # 处理参数

if length == 1:
    configPath = 'config.json'
elif length == 2:
    configPath = sys.argv[1]

'''
dictionary = {
    'Program Mode': 'usually',
    'Listen Address': '127.0.0.1',
    'Listen Port': '8080',
}
JsonOperation.save_dict_as_json(dictionary, 'config.json')
'''

def cpu_managers_start(script):

    CpuManager('CPU sensor', script)
    script['CPU sensor']['status'] = 'start'
    script['CPU sensor'].waitStart()

def memory_managers_start(script):

    MemoryManager('Memory sensor', script)
    script['Memory sensor']['status'] = 'start'
    script['Memory sensor'].waitStart()

def battery_managers_start(script):

    BetteryManager('Battery sensor', script)
    script['Battery sensor']['status'] = 'start'
    script['Battery sensor'].waitStart()

if __name__ =='__main__':

    TheConfig = JsonOperation.json_file_to_dict(configPath)
    manager = TheScript(TheConfig)

    manager['status'] = 'running'
    manager['printer'] = BasicPrinter()

    cpu_managers_start(manager)
    memory_managers_start(manager)
    battery_managers_start(manager)

    Server('Server', manager)
    manager['Server']['status'] = 'start'
    manager['Server'].waitStart()
    

    FTP_Server('FTP Server', manager)
    manager['FTP Server']['status'] = 'start'
    manager['FTP Server'].waitStart()


    
    if manager['Configure']['Program Mode'] == 'server':
        # 服务器模式
        while True:
            pass

    elif manager['Configure']['Program Mode'] == 'usually':
        # 通常模式
        command = ''
        while manager['status'] != 'shutdown':
            print('\n->',end='')
            CommandStr = input()
            result = commands(CommandStr, manager)
            manager['printer'].justPrint(result)

    


    manager['status'] = 'shutdown'