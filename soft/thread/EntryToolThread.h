#pragma once

#include <exception>
#include <stdexcept>
#include <stdio.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <errno.h>  
#include <poll.h>
#include <linux/input.h>  
#include "../script/DScript.h"
#include "../Printer/StandardPrinter.h"
#include "CommonThread.h"

/*录入工具从输入事件中读取字符，并且反映在输入框上面。
一个录入工具读取一个输入事件。
它们被录入管理器所创建、控制和关闭*/

namespace qing{
    class EntryToolThread: public CommonThread {
        /*录入器类型，继承自通用线程类型*/    
        public:
            
            EntryToolThread(StandardPrinter *printer, DScript *script, JsonString *jsonString, std::string name, std::string path) : CommonThread(printer, script, jsonString, name) {
                this->path = path;
            }/*构造函数，首先调用父类的构造*/
            
            /*暂时删除复制构造函数*/
            EntryToolThread(EntryToolThread&) = delete;
            
            ~EntryToolThread() {
                this->Destroy();
            }/*析构函数，如果不把线程的释放放到这里，将会导致纯虚函数错误*/
            
        protected:
        
        //----------------------------------------------------------

            static std::string prepare_label(std::string path){
                int l = std::string("/dev/input/").size();
                return path.substr(l, path.size() - l);
            }//prepare_label 
                   
        //----------------------------------------------------------
        
            void StopEvent() override {
                /*程序睡眠阶段的操作函数，线程等待*/
                this->suspend();
            }/*静止事件*/

            void WakeEvent() override{
                 
                /*准备地址、修改线程名*/
                CommonThread::SetLabel(prepare_label(this->path));
                
                /*配置输入事件*/
                this->fd = open(path.c_str(), O_RDONLY);  
		if (this->fd < 0){
		    this->Print("打开文件失败。");
		    this->stop();
		}//fail
		
		//success
		else this->run();
            }/*唤醒事件*/

            void LoopEvent() override{
                
                struct pollfd pfd;
                pfd.fd = this->fd;
                pfd.events = POLLIN;
                int ret = poll(&pfd, 1, this->t * 1000);
                if (ret == -1){
                    this->Print("set poll"); //error
                    this->stop();
                }
                else if(ret == 0) return; //timeout
                
                // 现在你可以尝试读取数据，它不会阻塞  
                struct input_event ev;  
                ssize_t n = read(this->fd, &ev, sizeof(ev));
                
                if (n == sizeof(ev)){
                    //this->Print(std::to_string(ev.value));               
                    if (ev.type == EV_KEY && ev.value == 1){
                                            
                        //handle
                        if(ev.code == 1) this->WriteScript("进程—状态","close");
                        this->Print(std::to_string(ev.code));
                        
                    }
                    
                } else if (n == -1) {  
                    if (errno == EAGAIN || errno == EWOULDBLOCK) return;// 没有数据可读，非阻塞调用
                    else {
                        this->Print("读取设备失败。");
                        this->stop();
                    }
                }
            
            }/*循环事件*/

            void ClearEvent() override{ 
                if(this->fd >= 0) ::close(this->fd);
            }/*清理事件*/

        private:

            std::string path;
            int fd = 0;
            float t = 0.4;

    };/*录入设备*/
}/*qing*/
