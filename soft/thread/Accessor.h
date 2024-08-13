#pragma once

#include <string.h>
#include <exception>
#include <stdexcept>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "../../cJSON/cJSON.h"
#include "../script/DScript.h"
#include "../script/JsonInterface.h"
#include "../Printer/StandardPrinter.h"
#include "../../network/Communication.h"
#include "CommonThread.h"

/*main.cpp*/
extern bool isDebug;
/* 基本的打字器 */
//extern qing::StandardPrinter *printer;
/* 全局脚本变量 */
//extern qing::DScript *script;


/*
@qing，2024-2-2：此模块为程序的网络访问程序。
*/

namespace qing {

    class Accessor: public CommonThread {
    /*访问器类型，继承自通用线程类型*/    
    public:
        /*构造函数，首先调用父类的构造*/
        Accessor(std::string name) : CommonThread(name) {}
	/*析构函数，关闭线程*/
	~Accessor(){
            if (this->chk() != SSHUT){
                this->close();//使线程自主关闭
            }
            this->WaitClose();//等待线程退出
	}
        /*暂时删除复制构造函数*/
        Accessor(Accessor&) = delete;
   

        void StopEvent() override {
            /*程序睡眠阶段的操作函数，线程等待*/
            this->suspend();
        }/*静止事件*/
      
        void WakeEvent() override {
            /*程序设置阶段的操作函数。*/
            try {
                /*根据配置文件中写的配置*/
                std::string path = ::script->Open("配置—访问器—地址");
                std::string port = ::script->Open("配置—访问器—端口");
                if (isDebug) printer->Print("", std::string("准备连接到目标：") + path + ":" + port);
                
                {
                    this->sock = socket(AF_INET, SOCK_STREAM, 0);
                    if (this->sock <= 0)
                        throw std::runtime_error(std::string("创建套接字失败：") + strerror(errno));
                        
                    struct in_addr testaddr;
                    if (inet_pton(AF_INET, path.c_str(), &testaddr) == 1) {
                
                        std::string ip = path;
                        memset(&serverip, 0, sizeof(struct sockaddr_in));
                    
                        serverip.sin_family = AF_INET;
                        serverip.sin_addr.s_addr = inet_addr(ip.c_str());
                        serverip.sin_port = htons(std::stoi(port));
                    
                    }
                
                    else {
                        //@陈昊元: Create socket by domain
                        hostent *host_ip = gethostbyname(path.c_str());
                        if(host_ip != NULL) {
                            if(host_ip->h_addr_list[0]) {
                            
                                //printf("域名 %s 的 ip = %s \n", domain.data(), ip.data());
                                std::string ip = inet_ntoa(*((struct in_addr*)host_ip->h_addr_list[0]));
                                memset(&serverip, 0, sizeof(struct sockaddr_in));
                            
                                serverip.sin_family = AF_INET;
                                serverip.sin_addr.s_addr = inet_addr(ip.c_str());
                                serverip.sin_port = htons(std::stoi(port));
                            
                            } else {
                                throw std::runtime_error(std::string("没有解析出IP。") + strerror(errno));
                            }
                        } else {
                            throw std::runtime_error(std::string("通过域名获取地址失败。") + strerror(errno));
                        }
                    }
                    
                }/* create local socket */
                
                /*@陈昊元: 设置套接字堵塞时间*/
                //clent->setBlockingTime(2);
                {
                
                    struct timeval t;
                    t.tv_sec = 1;
                    t.tv_usec = 0;
                
                    if (setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&t, sizeof(t)) < 0) {
                        throw std::runtime_error(std::string("设置套接字堵塞时间。") + strerror(errno));
                    }
                  
                    if (setsockopt(this->sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&t, sizeof(t)) < 0) {
                        throw std::runtime_error(std::string("设置套接字堵塞时间。") + strerror(errno));
                    }
                        
                } /* Set socket timeout */

		{
		    int sockfd;
		    int flag = 1;

		    //设置套接字选项以关闭Nagle算法
		    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
		}

                //printer->Print("","初始化成功");
                this->run();

            } catch (const std::exception& e) {
                /*线程设置失败*/
                ::printer->Print("",std::string("访问器线程设置失败： ") + e.what());
                this->stop();
		return;
            }/*设置阶段异常*/

        }/*配置事件*/
        
        
        void LoopEvent() override {
            /*程序运行阶段的操作函数。*/

            try {
            
                //成功时返回0
                if (0 != connect( this->sock, (struct sockaddr *)&serverip, sizeof(struct sockaddr_in))) {
                    throw std::runtime_error(std::string("套接字连接失败。") + strerror(errno));
                }
                if (isDebug) printer->Print("","已连接到目标IP \n");


                while (this->chk() == SRUNNING) { 
                
                    std::string msg_out;
                    if(isDebug) ::printer->Print(this->GetLabel(),"prepare");

                    /*准备简报，向临时简报中添加内容*/
                    {
                        cJSON *reportItem = cJSON_CreateObject();
                        std::string name = ::script->Open("简报—名字");
                        cJSON_AddStringToObject(reportItem, "名字", name.c_str());                    

                        /*将指示发送过去*/
                        cJSON *communicationItem = cJSON_CreateObject();
                        std::string sendstr = ::script->Open("指示—发送");
                        std::string instruction = sendstr == "" ? "关闭连接" : sendstr;
                        cJSON_AddStringToObject(communicationItem, "指示", instruction.c_str());
                        if (sendstr != "") ::script->Del("指示—发送"); 
			/*创建根结点*/
			cJSON *dataReport = cJSON_CreateObject();
                        cJSON_AddItemToObject(dataReport, "简报", reportItem);
                        cJSON_AddItemToObject(dataReport, "通信", communicationItem);
                        char *jsonString = cJSON_Print(dataReport);
                        msg_out = std::string(jsonString);
			/*清理*/
			free(jsonString);
			cJSON_Delete(dataReport);
                    }

                    if(isDebug) ::printer->Print(this->GetLabel(),"send");
                    /*文本形式发送*/
                    Communication::send_msg(this->sock, msg_out);

                    if(isDebug) ::printer->Print(this->GetLabel(),"recv");
                    /*文本形式接收*/
                    std::string recv_msg = Communication::recv_msg(this->sock);
                    //通用线程::打印(接收);
                    /*分析接受到的简报文本*/
                    std::string InstructionGot;

                    if (isDebug) ::printer->Print(this->GetLabel(),"analyze");
                    {
			/*解析收到的数据*/
			cJSON *dataReport = cJSON_Parse(recv_msg.c_str());

            	        cJSON *communicationItem = cJSON_GetObjectItem(dataReport, "通信");
            	        if(!communicationItem || !cJSON_IsObject(communicationItem))
                            throw std::runtime_error("收到错误信息。");

            	        cJSON *InstructionItem = cJSON_GetObjectItem(communicationItem, "指示");
            	        if(!InstructionItem || !cJSON_IsString(InstructionItem))
                            throw std::runtime_error("收到错误信息。");

			/*获取数据中的指示*/
            	        InstructionGot = std::string(InstructionItem->valuestring);

			/*清理*/
			cJSON_Delete(dataReport);
                    }

                    if		(InstructionGot == "关闭连接")	this->stop();
                    else if	(InstructionGot == "保持联络")	::script->Add("指示—发送", "保持联络");


                    usleep(100000);
		    //this->stop();
                }/*开始交流*/
                
                /*循环被跳出，即是收到了结束信号*/
                //::printer->Print("","成功。");

            }
	   
	    catch (Communication::CommuniError &e) {
                ::printer->Print("",std::string("短线重连，因为") + e.what());
                this->wake();
            }
	    catch (JsonInterface::JsonParseError &e) {
	        ::printer->Print("", std::string("解析失败，因为") + e.what());
		this->stop();
	    }
	    catch (std::exception& e) {
                ::printer->Print("",std::string("出现未知错误：") + e.what());
                this->stop();
	    }
	    /*通信过程中发生了异常*/

            /*重连的等待时间*/
            usleep(100000);

        } /*循环体*/
        
        
        void ClearEvent() override {
            /*程序清理阶段的操作函数。*/
        
            try { 
            
                if (this->sock > 0){
                    ::close(this->sock);
                    this->sock = -1;
                }
                
                //::printer:Print("","清除");
            } catch (std::exception& e) {
                ::printer->Print("",std::string("程序在清理阶段出现异常：") + e.what());
                this->stop(); return;
            }/*清理阶段出现了异常*/

        }/*清理事件*/

    private:
    
	//套接字
        int sock = -1;
        
	//这是一个用于存储要连接的服务器地址的结构体
        struct sockaddr_in serverip;
                               
    };/*访问器*/

}/*qing*/

