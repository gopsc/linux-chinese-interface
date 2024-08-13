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

//extern qing::StandardPrinter *printer;
//extern qing::DScript *script;

/*录入工具从输入事件中读取字符，并且反映在输入框上面。
一个录入工具读取一个输入事件。
它们被录入管理器所创建、控制和关闭*/

namespace qing{
    class EntryToolThread: public CommonThread {
        /*录入器类型，继承自通用线程类型*/    
        public:
            
            EntryToolThread(std::string name, std::string path) : CommonThread(name) {
                this->path = path;
            }/*构造函数，首先调用父类的构造*/
	    /*析构函数，关闭线程*/
	    ~EntryToolThread(){
                if (this->chk() != SSHUT){
                    this->close();//使线程自主关闭
                }
                this->WaitClose();//等待线程退出
	    }
            
            /*暂时删除复制构造函数*/
            EntryToolThread(EntryToolThread&) = delete;
            
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
		    ::printer->Print("","打开文件失败。");
		    this->stop();
		}//fail
		
		//success
		else this->run();
            }/*唤醒事件*/

            void LoopEvent() override{
                
                struct pollfd pfd;
                pfd.fd = this->fd;
                pfd.events = POLLIN;
                int ret = poll(&pfd, 1, 0.4 * 1000);
                if (ret == -1){
                    ::printer->Print("","设置poll出错。"); //error
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
                        if(ev.code == 1) ::script->Add("进程—状态","终止");
                        ::printer->Print(this->GetLabel(), std::to_string(ev.code));
                        
                    }
                    
                } else if (n == -1) {  
                    if (errno == EAGAIN || errno == EWOULDBLOCK) return;// 没有数据可读，非阻塞调用
                    else {
                        ::printer->Print(this->GetLabel(),"读取设备失败。");
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

    };/*录入设备*/
}/*qing*/
