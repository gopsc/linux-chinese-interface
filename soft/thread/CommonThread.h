#pragma once

#include <iostream>
#include <stdexcept>
#include <thread>
#include "../../type.h"
#include "../model/fsm.h"
#include "../Printer/StandardPrinter.h"
#include "../script/JsonString.h"
#include "../script/DScript.h"

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

namespace qing {
    class DScript;
    class CommonThread: public Fsm{
        //一个通常情况下的线程类型        
        public:  
            //构造函数
            CommonThread(StandardPrinter *printer, DScript *script, JsonString *jsonString, std::string name);
            //如果把析构函数放在这里，将会导致纯虚函数错误
            //...
            //对线程名字的设置
            void SetLabel(std::string name);
            //对线程名字的获取
            std::string GetLabel();
            //获取打印类的地址
            StandardPrinter* GetPrinter();
            //获取csc字典的地址
            DScript* GetScript();
            JsonString* GetJsonString();
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
        
        protected:
            //-----------------------------------------------------------
            //用来被构造函数和析构函数调用的方法
        
            //结束线程的函数
            void Destroy();
        
            //-----------------------------------------------------------
            //将数据发送到打印机类
        
            //以自己的名字为开头，将数据打印
            void Print(std::string msg);
            //直接打印
            void PrintOnly(std::string msg);

            //-----------------------------------------------------------
            //读写脚本
            std::string ReadScript(std::string key){
                return this->script->Open(key);
            }//读脚本

            void WriteScript(std::string k, std::string v){
                this->script->Add(k,v);
            }//写脚本
            //-----------------------------------------------------------
        
        private:
        
            //线程的名字
            std::string label;
            //用于整理输出的类
            StandardPrinter *printer;
            //用于存放配置文件的CSC类
            DScript *script;     
            JsonString *jsonString;   
            //用来存放该子线程的标识符。
            std::thread *thread = NULL;

            //-----------------------------------------------------------
            //通常情况下，线程的主函数。
        
            static void main(CommonThread *th, StandardPrinter *printer, std::string name);

            //程序睡眠阶段的操作函数。
            virtual void StopEvent() = 0;
            //程序设置阶段的操作函数。
            virtual void WakeEvent() = 0;
            //程序运行阶段的操作函数。
            virtual void LoopEvent() = 0;
            //程序清理阶段的操作函数。
            virtual void ClearEvent() = 0;

            //可以将操作函数作为一种事件，可以从外界传入写好的事件函数，
            //那么就需要将操作函数设定为静态函数

        };
}
