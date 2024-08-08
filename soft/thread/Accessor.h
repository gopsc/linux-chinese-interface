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
#include "../script/DScript.h"
#include "../Printer/StandardPrinter.h"
#include "../../network/Communication.h"
#include "CommonThread.h"

/*
@qing，2024-2-2：此模块为程序的网络访问程序。
*/

namespace qing {

    class Accessor: public CommonThread {
    /*访问器类型，继承自通用线程类型*/    
    public:
        /*构造函数，首先调用父类的构造*/
        Accessor(StandardPrinter *printer, DScript *script, JsonString *jsonString, std::string name) : CommonThread(printer, script, jsonString, name) {}
        /*暂时删除复制构造函数*/
        Accessor(Accessor&) = delete;
        
        ~Accessor() {
            this->Destroy();
        }/*析构函数，如果不把线程的释放放到这里，将会导致纯虚函数错误*/
        
    protected:
    
        void StopEvent() override {
            /*程序睡眠阶段的操作函数，线程等待*/
            this->suspend();
        }/*静止事件*/
      
        void WakeEvent() override {
            /*程序设置阶段的操作函数。*/
            try {
                /*根据配置文件中写的配置*/
                std::string path = CommonThread::GetScript()->Open("配置—访问器—地址");
                std::string port = CommonThread::GetScript()->Open("配置—访问器—端口");
                //CommonThread::Print(std::string("准备连接到目标：") + path + ":" + port);
                
                /*在堆上创建一个Linux客户端类型*/
                //clent = new LinuxClient(path, port);
            
                {
                    this->sock = socket(AF_INET, SOCK_STREAM, 0);
                    if (this->sock <= 0)
                        throw std::runtime_error(std::string("createSocket: ") + strerror(errno));
                        
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
                                throw std::runtime_error(std::string("Parse no ip: ") + strerror(errno));
                            }
                        } else {
                            throw std::runtime_error(std::string("gethostbyname: ") + strerror(errno));
                        }
                    }
                    
                }/* create local socket */
                
                /*@陈昊元: 设置套接字堵塞时间*/
                //clent->setBlockingTime(2);
                {
                
                    struct timeval t;
                    t.tv_sec = 2;
                    t.tv_usec = 0;
                
                    if (setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&t, sizeof(t)) < 0) {
                        throw std::runtime_error(std::string("Set socket timeout: ") + strerror(errno));
                    }
                  
                    if (setsockopt(this->sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&t, sizeof(t)) < 0) {
                        throw std::runtime_error(std::string("Set socket timeout: ") + strerror(errno));
                    }
                        
                } /* Set socket timeout */


		{
		    int sockfd;
		    int flag = 1;

		    //设置套接字选项以关闭Nagle算法
		    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
		}


                //通用线程::打印("初始化成功");
                this->run();

            } catch (const std::exception& e) {
                /*线程设置失败*/
                CommonThread::Print(std::string("线程设置失败： ") + e.what());
                this->stop(); return;
            }/*设置阶段异常*/

        }/*配置事件*/
        
        
        void LoopEvent() override {
            /*程序运行阶段的操作函数。*/

            try {
            
                /*开始连接*/
                //clent->StartConnect();
                
                // Is't only success as 0 return
                if (0 != connect( this->sock, (struct sockaddr *)&serverip, sizeof(struct sockaddr_in))) {
                    throw std::runtime_error(std::string("connect failed: ") + strerror(errno));
                }
                //printf("已连接到目标IP \n");


                while (this->chk() == SRUNNING) { 
                
                    std::string msg_out;
//this->Print("prepare");

                    /*准备简报，向临时简报中添加内容*/
                    {
                        //DScript tmp1;
                        //tmp1.Add("简报—名字", CommonThread::GetScript()->Open("简报—名字"));
                        cJSON *reportItem = cJSON_CreateObject();
                        std::string name = CommonThread::GetScript()->Open("简报—名字");
                        cJSON_AddStringToObject(reportItem, "名字", name.c_str());                    

                        /*将指示发送过去*/
		        		//std::string sendstr = CommonThread::ReadScript("指示—发送");
                        //tmp1.Add("通信—指示", sendstr == "" ? "关闭连接" : sendstr);
                        cJSON *communicationItem = cJSON_CreateObject();
                        std::string sendstr = CommonThread::ReadScript("指示—发送");
                        std::string instruction = sendstr == "" ? "关闭连接" : sendstr;
                        cJSON_AddStringToObject(communicationItem, "指示", instruction.c_str());
                        if (sendstr != "") CommonThread::GetScript()->Del("指示—发送"); 

                        JsonString Json_DataReport_Out;
                        cJSON_AddItemToObject(Json_DataReport_Out.get(), "简报", reportItem);
                        cJSON_AddItemToObject(Json_DataReport_Out.get(), "通信", communicationItem);

                        msg_out = Json_DataReport_Out.print();
                    }

//this->Print("send");
                    /*文本形式发送*/
                    //clent->send_msg(clent->getSocket(), tmp1.toStr());
                    //clent->send_msg(clent->getSocket(), msg_out);
                    Communication::send_msg(this->sock, msg_out);

//this->Print("recv");
                    /*文本形式接收*/
                    //std::string recv_msg = clent->recv_msg(clent->getSocket());
                    std::string recv_msg = Communication::recv_msg(this->sock);
//this->Print(recv_msg);
                    //通用线程::打印(接收);
                    /*分析接受到的简报文本*/
                    std::string InstructionGot;

//this->Print("analyze");
                    {
                        //DScript tmp2(recv_msg);
                        //std::string InstructionGot = tmp2.Open("通信—指示");
                        JsonString Json_DataReport_In(recv_msg);
                    	cJSON *cjsonItem = Json_DataReport_In.get();

            	        cJSON *communicationItem = cJSON_GetObjectItem(cjsonItem, "通信");
            	        if(!communicationItem || !cJSON_IsObject(communicationItem)) throw std::runtime_error("收到错误信息。");

            	        cJSON *InstructionItem = cJSON_GetObjectItem(communicationItem, "指示");
            	        if(!InstructionItem || !cJSON_IsString(InstructionItem)) throw std::runtime_error("收到错误信息。");

            	        InstructionGot = std::string(InstructionItem->valuestring);
                    }

                    if		(InstructionGot == "关闭连接")	this->stop();
                    else if	(InstructionGot == "保持联络")	CommonThread::WriteScript("指示—发送", "保持联络");
                    //usleep(100000);
                }/*开始交流*/
                
                /*循环被跳出，即是收到了结束信号*/
                //CommonThread::Print("成功。");

            } catch (std::exception& e) {
                CommonThread::Print(std::string("通信遇到异常：") + e.what());
                this->stop();
            }/*通信过程中发生了异常*/

            /*重连的等待时间*/
            usleep(100000);

        } /*循环体*/
        
        
        void ClearEvent() override {
            /*程序清理阶段的操作函数。*/
        
            try { 
            
                /*如果Linux服务器类型已经被设置过了
                就将其析构，如果没有什么也不做*/
                /*
                if (this->clent != NULL){
                    delete(this->clent);
                    this->clent = NULL;
                }
                */
                if (this->sock > 0){
                    ::close(this->sock);
                    this->sock = -1;
                }
                
                //CommonThread::Print("清除");
            } catch (std::exception& e) {
                CommonThread::Print(std::string("程序在清理阶段出现异常：") + e.what());
                this->stop(); return;
            }/*清理阶段出现了异常*/

        }/*清理事件*/

    private:
        /*声明一个Linux客户端类型的指针*/
        //LinuxClient *clent = NULL;
    
        int sock = -1;
        
        //A struct type for recording 
	//the address of the server
	//to connect to
        struct sockaddr_in serverip;
                               
    };/*访问器*/

}/*qing*/

