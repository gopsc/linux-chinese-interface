#pragma once

#include <iostream>
#include <stdexcept>
#include <thread>
#include "../../type.h"
#include "../Printer/StandardPrinter.h"
#include "../script/DScript.h"
#include "BasicThread.h"

/*
@qing：包含一个简易的多线程控制机制。
需要共享内存的变量和函数声明为全局变量（通常需要带锁），
线程中单独使用的变量和函数声明到属于它的命名空间中。
每个线程的主函数包括了对其的流程控制代码。
stop，setup，loop，clean函数中填入每个状态下的执行代码。
其中需要用到的函数，又实现在后边。
*/

/*
@qing：通用的信使类型，抽象类，含有通用型主函数的定义
*/
extern qing::StandardPrinter *printer;
extern qing::DScript *script;

namespace qing {
    //线程的主函数
    void thread_main(BasicThread *th);

    //前向声明脚本类型
    class DScript;
    class CommonThread: public BasicThread {
        //一个持有资源线程的状态机类型        
        public:  
            //构造函数.
	    //没有必要输入打印机、脚本、线程的名字，放到全局变量
            CommonThread(std::string name);
	    //暂时删除复制构造函数
	    CommonThread(CommonThread&) = delete;
            //如果把析构函数放在这里，将会导致纯虚函数错误
            virtual ~CommonThread();

            //对线程名字的设置
            void SetLabel(std::string name);
            //对线程名字的获取
            std::string GetLabel();
            //-----------------------------------------------------------
            bool WaitStart(float time) {
                //等待线程启动完毕
                //线程启动失败返回失败。线程启动成功或者现程结束返回成功。
                int n = time * 1000000;
                for(unsigned long i=0; i<n; i++){
                    enum Stat stat = this->chk();
                    if		(stat == SSTOP)				return false;
                    else if	(stat == SRUNNING || stat == SSHUT)	return true;
                    else if     (stat == SSTART)                        usleep(10000);
                }//按次循环
                return false;
            }//等线程开始
        
            void WaitClose() {
                thread->join();
            }//等线程关闭


            virtual void StopEvent() override{
		printer->Print(this->GetLabel(), "注意，原始事件函数被调用。");
	        this->suspend();
	    }
            virtual void WakeEvent() override{
		printer->Print(this->GetLabel(), "注意，原始事件函数被调用。");
	        this->run();
	    }
	    virtual void LoopEvent() override{
		printer->Print(this->GetLabel(), "注意，原始事件函数被调用。");
	        usleep(10000);
	    }
	    virtual void ClearEvent() override {
		printer->Print(this->GetLabel(), "注意，原始事件函数被调用。");
	    }
        
        private:
        
            //线程的名字
            std::string label;
            //用来存放该子线程的标识符。
            std::thread *thread = NULL;
        };
}
