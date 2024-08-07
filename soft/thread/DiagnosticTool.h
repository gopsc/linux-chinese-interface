#pragma once

#include <cstdint>
#include <iostream>
#include"../../hard/CpuInterface.h"
#include"../../hard/MemInterface.h"
#include"../../hard/BattInterface.h"
#include "../../hard/graph/colors.h"
#include "../../hard/graph/Area/Area.h"
#include "../Printer/StandardPrinter.h"
#include "../Painter/CmdPainter.h"
#include "CommonThread.h"
#include "../script/DScript.h"
/*
* @qing：改为监管系统情况的函数，
* 也可以传入一个回调函数以在设置阶段执行
*/

extern qing::CmdPainter *painter;
extern int ready;
namespace qing{
    class DiagnosticTool: public CommonThread {
    /*开发测试用*/

    public:
        /*构造函数*/
        DiagnosticTool (StandardPrinter *printer, DScript *script, JsonString *jsonString, std::string name) : CommonThread(printer, script, jsonString, name)  {}
        /*暂时删除复制构造函数*/
        DiagnosticTool(DiagnosticTool&) = delete;
        
        ~DiagnosticTool(){
            this->Destroy();
        }/*析构函数，必须在这里调用*/
        
    protected:
        void StopEvent() override {
            /*程序睡眠阶段的操作函数。*/
            /*线程挂起*/
            this->suspend();
        }/*静止事件*/

        void WakeEvent() override{
            /*程序在设置阶段的操作函数。*/
            this->run();
        }/*配置事件*/
        
        
        void LoopEvent() override {
            /*程序运行阶段的操作函数。*/
            /*刷新绘图器中的缓冲区*/
            //painter->lk.lock();
            
            Area *a0 = painter->a2;    //状态栏
            a0->paint(BLACK32);
            
            Area *a1 = painter->a2_1;  //处理器状态
            int cpUsage = CpuInterface::GetUsage();
            std::string str_cpUsage = std::to_string(cpUsage);
            std::string info_cpUsage = "芯片 " + str_cpUsage + "%" ;
            CommonThread::GetScript()->Add("简报—处理器", str_cpUsage);
            char *ansi_cpUsage = string::utf8_to_ansi((char*)info_cpUsage.c_str());
            a1->paint(BLACK32);
            __uint32_t clr1  =  cpUsage < 40 ? GREEN32 : cpUsage < 80 ? YELLOW32 : RED32;
            a1->print(ansi_cpUsage, painter->char_w1, painter->char_h1, clr1);
            free(ansi_cpUsage);
            
            Area *a2 = painter->a2_2;  //温度状态
            int cpuTemp = CpuInterface::GetTemp();
            std::string str_cpuTemp = std::to_string(cpuTemp);
            std::string info_cpuTemp = "温度 " + str_cpuTemp + "摄" ;
            CommonThread::GetScript()->Add("简报—温度",  str_cpuTemp);
            char *ansi_cpuTemp   = string::utf8_to_ansi((char*)info_cpuTemp.c_str());
            a2->paint(BLACK32);
            __uint32_t clr2  =  cpuTemp < 48 ? GREEN32 : cpuTemp < 60 ? YELLOW32 : RED32;
            a2->print(ansi_cpuTemp,   painter->char_w1, painter->char_h1, clr2);
            free(ansi_cpuTemp);

            Area *a3 = painter->a2_3;  //内存状态
            int memUsage = MemInterface::GetUsage();
            std::string str_memUsage = std::to_string(memUsage);
            std::string info_memUsage = "内存 " + str_memUsage + "%" ;
            CommonThread::GetScript()->Add("简报—内存",  str_memUsage);
            char *ansi_memUsage   = string::utf8_to_ansi((char*)info_memUsage.c_str());
            a3->paint(BLACK32);
            __uint32_t clr3  =  memUsage < 40 ? GREEN32 : memUsage < 80 ? YELLOW32 : RED32;
            a3->print(ansi_memUsage,   painter->char_w1, painter->char_h1, clr3);
            free(ansi_memUsage);

            Area *a4 = painter->a2_4;  //电池状态            
            int battUsage = BattInterface::GetUsage();
            std::string str_battUsage = std::to_string(battUsage);
            std::string info_battUsage = "电池 " + str_battUsage + "%" ;
            CommonThread::GetScript()->Add("简报—电池",  str_battUsage);
            char *ansi_battUsage   = string::utf8_to_ansi((char*)info_battUsage.c_str());
            a4->paint(BLACK32);
            __uint32_t clr4  =  battUsage > 80 ? GREEN32 : battUsage > 40 ? YELLOW32 : RED32;
            a4->print(ansi_battUsage,   painter->char_w1, painter->char_h1, clr4);
            free(ansi_battUsage);

            Area *a5 = painter->a2_5;  //充电状态
            std::string chargingStat =  BattInterface::GetStatus();
            CommonThread::GetScript()->Add("简报—充电", chargingStat);
            char *ansi_chargingStat   = string::utf8_to_ansi((char*)chargingStat.c_str());            
            a5->paint(BLACK32);
            __uint32_t clr5  =  chargingStat == "已充满电" ? GREEN32 :
                             chargingStat == "正在充电" || chargingStat == "没有电池" ? YELLOW32 : RED32;
            a5->print(ansi_chargingStat,   painter->char_w1, painter->char_h1, clr5);
            free(ansi_chargingStat);
            
            //painter->Paint();
            //painter->lk.unlock();
            if (ready<100) ready++;
            usleep(100000);
        }//循环体

        void ClearEvent() override {
            //程序在清理阶段的操作函数。
            CommonThread::Print("清除。");
        }//清除事件

    private:

    };
}
