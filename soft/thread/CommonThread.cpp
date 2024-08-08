
#include "CommonThread.h"

namespace qing {
	 
    CommonThread::CommonThread(StandardPrinter *printer, DScript *script, JsonString *jsonString, std::string name) {
        /*构造函数，完成数据域的设置。*/
        this->label = name;
        this->printer = printer;
        this->script = script;
        this->jsonString = jsonString;
        //创建线程的函数
        this->thread = new std::thread(this->main, this, this->printer, this->label);
    }
    
    void CommonThread::Destroy() {
        //结束线程的函数
        
        if (this->thread != NULL){
            //清除线程

std::cout << std::string("Close:\t") + this->label + "\n";
            
            if (this->chk() != SSHUT){
                this->close();//使线程自主关闭
            }
            this->WaitClose();//等待线程退出
            delete this->thread;//删除线程类

        }
            
    } //销毁线程
    
        
    //-----------------------------------------------------------
    //访问器函数

    void CommonThread::SetLabel(std::string name) {
        //对线程名字的设置
        this->label = name;
    }
    
    std::string CommonThread::GetLabel() {
        //对线程名字的获取
        return this->label;
    }
		
    StandardPrinter* CommonThread::GetPrinter() {
        //获取打印类的地址
        return this->printer;
    }//取打字机
    
    DScript* CommonThread::GetScript() {
        //获取csc字典的地址
        return this->script;
    }//取脚本

    JsonString *CommonThread::GetJsonString(){
        return this->jsonString;
    }

    //-----------------------------------------------------------
    //将数据发送到打印机类        
             
    void CommonThread::Print(std::string msg) {
        //以自己的名字为开头，将数据打印
        this->printer->Print(this->GetLabel(), msg);
    }
   
    void CommonThread::PrintOnly(std::string msg) {
        //直接打印
        this->printer->Print("", msg);
    }

    //-----------------------------------------------------------
    /*通常情况下，线程的主函数。*/
        
    void CommonThread::main(CommonThread *th, StandardPrinter *printer, std::string name) {
        //printer->Print(name, "线程启动。");
            
        while (th->chk() != SSHUT) {
            /*进入回字形循环，如果不是进入关闭态
            内层循环会一直进行。*/         
            //printer->打印(name, "进入循环。");
                    
            if (th->chk() == SSTOP){
                /*内层循环，静止状态*/
                th->StopEvent();
            }

            if (th->chk() == SSTART) {
                /*由静止状态退出时，如果进入开始状态
                则进入单次的设置操作*/
                //printer->Print(name,"被唤醒。");
                th->WakeEvent();
            }

            if (th->chk() == SRUNNING) {
                /*由设置阶段退出时，如果是运行状态，则发出提示*/
                //printer->Print(name,"开始运转。");
            }

            while (th->chk() == SRUNNING) {
                //进入运行态
                th->LoopEvent();
            }

            /*循环末尾，进行一次清理
            由于之前可能是由静止态直接切换到关闭工作
            在清理时需要检查成员变量是否是被设置过的*/
            //printer->Print(name, "开始清理。");
            th->ClearEvent();
        }
            
        //printer->Print(name, "线程退出。");
    }//入口

}
