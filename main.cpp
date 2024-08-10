/*@qing，20240714：（
 * 想象你是个中国人，平凡的你刚刚拿到一台设备
 * 满屏幕的英文字符是不是让你束手无策？
 * 所以我要写一个简单的能够快速部署到所有
 * 设备上面的中文界面。
 * 所有的设备都可以激活这个功能
 * 这样就所有人都能看懂并且操作设备了。
 * ）
 */

//=========================================================================================
//Linux模式

#ifdef __linux__
#include <unistd.h>

#include "soft/system-call/FilesystemOp.h"
#include "soft/script/DScript.h"
#include "soft/terminal/DisableEcho.h"
#include "soft/terminal/HideCursor.h"
#include "soft/Printer/StandardPrinter.h"
#include "soft/Painter/CmdPainter.h"
#include "soft/thread/CommonThread.h"
#include "soft/thread/EntryManagerTh.h"
#include "soft/thread/CompileManager.h"
#include "soft/thread/Accessor.h"
#include "soft/thread/Reciver.h"

//-----------------------------------------------------------------------------------------
//C++程序入口
#ifdef __cplusplus
#include <iostream>
#include <stdexcept>
#include <vector>

/*
 * @qing，20240407：
 * 宣言机制。在创建该类型后，自动对
 * 环境进行设置，在对象销毁后，
 * 自动还原为之前的状态，
 *
 * 因为配置顺序，
 * 可能规定创建释放顺序
 */

#ifndef DEBUG    
//PrimaryTerminal 原始终端;
qing::DisableEcho disableEcho;
qing::HideCursor  hideCursor;
#endif

/* 打印机 */
qing::StandardPrinter *printer = new qing::StandardPrinter(1024, 1);
/*全局的脚本*/
qing::DScript *script = NULL;
/* 绘图器 */
qing::CmdPainter *painter = NULL;
/* 线程池 */
std::vector<qing::CommonThread*> pool(10, NULL);
    

int main(int argc, char **argv) {

/*
 * @qing，20240714：（
 * 设置配置文件路径。参[0]是被执行的文件，参[1]可传入配置文件。
 * ）
 */
    std::string confPath = "/etc/lci/configure.csc";
    if (argc == 2) confPath = argv[1];
  
    script = new qing::DScript(qing::FilesystemOp::Readfile(confPath));
    painter = new qing::CmdPainter(printer, script, "/etc/lci/logo.jpg");


/*
 * @qing，20240714：（
 * 切换到8号图像控制台。
 * 在大部分的Linux系统中，该控制台是空的。红帽系统似乎没有8号控制台。
 *
 * 封装成系统调用。
 * 封装成声明类型。
 * ）
 */
    pool[0] = new qing::EntryManagerTh(printer, script, "输入管1");
    pool[1] = new qing::Reciver(printer, script, "接收器1");
    pool[2] = new qing::Accessor(printer, script, "访问器1");
    
/*
    //编译线程
    pool[3] = new qing::CompileManager(printer, script, "管理器1");
*/    
    for (int i=0; i<pool.size(); ++i){
        if(pool[i]){
            pool[i]->wake();
            pool[i]->WaitStart(1);
        }
    }

#ifndef DEBUG
    system("chvt 8");
#endif

    printer->Print("", "你好，操作员。系统已经开始运转。\n");
    sleep(1);

    //script->Add("进程—状态", "运行");
    /*用脚本来控制开关状态。*/
    while(script->Open("进程—状态") == "运行") {
        usleep(10000);
    }

#ifndef DEBUG
    system("chvt 1");
#endif
    
    for (int i=0; i<pool.size(); i++){
        if(pool[i]){
	    delete pool[i];
	    pool[i] = NULL;
	}
    }
    //sleep(1);
    delete painter;
    delete script;
    delete printer;

    std::cout << "\nprogram shut.\n";
    while(true) usleep(100000);
    return 0;
}

#endif //__cplusplus
#endif //__linux__


//=========================================================================================
//Windows模式

#ifdef _WIN32
#include<Windows.h>

#include "中文化.h"
#include "软件/数据/字符流.h"
#include "软件/数据/字符串.h"

//-----------------------------------------------------------------------------------------
//C++程序入口

#ifdef __cplusplus
#include <iostream>
#include <stdexcept>

//使用 命名空间 夕;
整数 入口(整数 数量, 字符 **参数)
{
    //第一个参数就是可执行文件本身，
    //第二个参数是csc配置文件的地址
    //用span
    如果 (数量 != 2)
        抛出异常 std::runtime_error("请输入一个CSC文件名作为参数。");

    判断循环(真){
        夕::字符流 流1("你好，世界。", 夕::字符流::从数组);
        夕::字符流 流2("hello,world!", 夕::字符流::从数组);
        夕::字符串 串1 = 流1.到字符串().到ANSI();
        夕::字符串 串2 = 流2.到字符串().到ANSI();
        夕::字符串 文 = 串1 + 串2;
        std::cout << 文.取数据() << std::endl;
    }


    返回 0;
}

#endif //__cplusplus
#endif //_WIN32
