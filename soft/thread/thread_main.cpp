#include "../Printer/StandardPrinter.h"
#include "BasicThread.h"
extern qing::StandardPrinter *printer;


namespace qing {
//-----------------------------------------------------------
/*通常情况下，线程的主函数。*/
        
    void thread_main(BasicThread *th) {
            
        while (th->chk() != SSHUT) {
            /*进入回字形循环，如果不是进入关闭态
            内层循环会一直进行。*/         
                    
            if (th->chk() == SSTOP){
                /*内层循环，静止状态*/
                th->StopEvent();
            }

            if (th->chk() == SSTART) {
                /*由静止状态退出时，如果进入开始状态
                则进入单次的设置操作*/
                th->WakeEvent();
            }

            if (th->chk() == SRUNNING) {
                /*由设置阶段退出时，如果是运行状态，则发出提示*/
            }

            while (th->chk() == SRUNNING) {
                //进入运行态
                th->LoopEvent();
            }

            /*循环末尾，进行一次清理
            由于之前可能是由静止态直接切换到关闭工作
            在清理时需要检查成员变量是否是被设置过的*/
            th->ClearEvent();
        }
            
    }//入口

}
