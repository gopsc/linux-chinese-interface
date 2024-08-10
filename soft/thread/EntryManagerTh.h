#pragma once

#include<unistd.h>
#include<iostream>
#include<vector>
#include<filesystem>
#include<string>

#include "../script/DScript.h"
#include "../Printer/StandardPrinter.h"
#include "CommonThread.h"
#include "EntryToolThread.h"

/*录入工具从输入事件中读取字符，并且反映在输入框上面。
一个录入工具读取一个输入事件。
它们被录入管理器所创建、控制和关闭*/

namespace qing{
    class EntryManagerTh: public CommonThread {
        /*录入器类型，继承自通用线程类型*/    
        public:
            
            EntryManagerTh(StandardPrinter *printer, DScript *script, std::string name) : CommonThread(printer, script, name) {
            }/*构造函数，首先调用父类的构造*/
            
            /*暂时删除复制构造函数*/
            EntryManagerTh(EntryManagerTh&) = delete;
            
        
            void StopEvent() override {
                /*程序睡眠阶段的操作函数，线程等待*/
                this->suspend();
            }/*静止事件*/

            void WakeEvent() override{
                // /dev/input/eventX
                // 设备文件
                // /proc/bus/input/devices
                // 设备描述

                std::string dirPath = "/dev/input";
                for(const auto& entry : std::filesystem::directory_iterator(dirPath)){
                    std::string path = entry.path().string();
                    //this->Print(path);

                      //if(std::filesystem::is_block_file(entry)){
                      if(path.find("/dev/input/event") == 0){

                        /*准备线程的名字*/
                        std::string name = std::string("录入工具") + std::to_string(num++);
                        /*启动线程*/
                        EntryToolThread *entryTool = new EntryToolThread(CommonThread::GetPrinter(), CommonThread::GetScript(), name, path);
                        entryTool->wake(); entryTool->WaitStart(1);
                        /*推入线程池*/
                        this->pool.push_back(entryTool);
                    }
                }

                this->run();
            }/*唤醒事件*/

            void LoopEvent() override{
                //
                usleep(10000);
            }/*循环事件*/

            void ClearEvent() override{
                for(int i = pool.size() - 1; i > 0; --i){
                    delete(pool[i]);
                }
            }/*清理事件*/

        private:
            /*计数器*/
            size_t num = 0;
            /*线程池*/
            std::vector<EntryToolThread*> pool;

    };/*录入设备*/
}/*qing*/
