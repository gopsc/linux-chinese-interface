
import threading

class TheScript:
    '''
    哈希域，用于记录程序的状态。就像键值对语言中的一个对象一样。
    让需要设置此域的脚本继承此类，继承此类的脚本，可以操作变量池。

    可能添加其他功能，比如为这个脚本添加访问权限。比如，只有管理员可以修改变量池

    @qing,20240610:在Script类型的字典成员中，有可能包含了一个普通的字典，而不是另一个Script类型
    在这种情况下，对其的访问将不是线程安全的，所以需要创建哈希子域
    '''

    def __init__(self, container) -> None:
        # 构造函数

        self.lock = threading.Lock()
        # 锁，保证每个脚本的操作都处于临界区
        self.container = container
        # 变量池，用于存储可变量。

    def __del__(self):
        #析构函数
        pass

    def __setitem__(self, key, value):
        # 操作函数，通过下标赋值
        with self.lock:
            #进入临界区

            self.container[key] = value
            #为该下标赋值

    def __getitem__(self, key):
        # 操作函数，返回对字典元素的引用
        with self.lock:
            # 进入临界区

            return self.container[key]
            # 返回对字典元素的引用

    def __delitem__(self, key):
        # 操作函数，删除字典元素
        with self.lock:
            # 进入临界区
            
            del self.container[key]
            # 删除字典元素

    def __str__(self):
        # 操作函数，返回字典键名列表的字符串

        with self.lock:
            # 进入临界区

            return '\n'.join(self.container.keys())
            #组合出结果

    def __len__(self):
        # 操作函数，返回字典键名的数量

        with self.lock:
            # 进入临界区

            return len(self.container.keys())
            # 返回结果
