#pragma once

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include "../../network/Communication.h"
#include "../script/DScript.h"
#include "../Printer/StandardPrinter.h"
#include "CommonThread.h"

namespace qing {
	
	class Connector: public CommonThread{
	    /*服务器获取连接后，创建该线程与客户端通信
        连接关闭后，也许通过信号机制删除连接池中的连接*/
	public:
	    Connector(StandardPrinter* printer, DScript* script, JsonString *jsonString, std::string name, int sock, sockaddr_in addr): CommonThread(printer, script, jsonString, name) {
			/*构造函数*/
			this->sock = sock;
			this->addr = addr;
		}
		
		/*暂时删除复制构造函数*/
		Connector(Connector&) = delete;
		
        ~Connector() {
            /*析构函数，如果不把线程的释放放到这里，将会导致纯虚函数错误*/
            this->Destroy();
        }/*析构函数*/

    private:
    
        //ClientSocket *sock;
        int sock;
        sockaddr_in addr;
        
        void StopEvent() override {
            /*程序睡眠阶段的操作函数，什么也不做*/
            this->suspend();
        }/*静止事件*/
        
            
        void WakeEvent() override {
            /*程序设置阶段的操作函数。*/

            try {
                /* 获取连接者的IP地址 */
                //CommonThread::Print(std::string("有客户端连接。") + sock->getIp());
                //CommonThread::SetLabel(sock->getIp());
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
                int port = ntohs(addr.sin_port);
                /* 设置为该线程的标签 */
                std::string s = std::string(ip) + ":" + std::to_string(port);
                CommonThread::SetLabel(s);
                

                //sock->setBlockTime(2);
                {
                    struct timeval t;
                    t.tv_sec = 2;
                    t.tv_usec = 0;
                    
                    if (setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t, sizeof(t)) < 0)
                        throw std::runtime_error(std::string("Set socket timeout: ") + strerror(errno));
                        
                    if (setsockopt(this->sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&t, sizeof(t)) < 0)
                        throw std::runtime_error(std::string("Set socket timeout: ") + strerror(errno));
                }

		{
		    int sockfd;
		    int flag = 1;

		    //设置套接字选项以关闭Nagle算法
		    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
		}


                //通用线程::打印("初始化成功。");
                this->run();
            } catch (std::exception& e) {/*设置阶段异常*/
                CommonThread::Print(std::string("程序在设置阶段出现异常：") + e.what());
                this->stop(); return;
            }/*设置阶段异常*/
            
		}/*配置事件*/
		
             
        void LoopEvent() override {
            /*程序运行阶段的操作函数。开始交流*/

            try {
                /*一会儿我们将其改为以事件队列驱动*/
//this->Print("recive");
                /*接收数据*/
                std::string RcvData = "";
		RcvData = Communication::recv_msg(this->sock);
                //通用线程::打印(收到数据);
		if (RcvData == "") throw std::runtime_error("未收到数据。");
                    
                /*读取接收到的csc*/
                //DScript DataReport_in(RcvData);
		std::string InstructionGot;

//this->Print("analyze");
                //--------------------------------------------------------------------------------
                /*检查简报中的指示*/
                {
                    
            	    //std::string InstructionGot = DataReport_in.Open("通信—指示");
                    JsonString Json_DataReport_In(RcvData);
                	cJSON *cjsonItem = Json_DataReport_In.get();

            	    cJSON *communicationItem = cJSON_GetObjectItem(cjsonItem, "通信");
            	    if(!communicationItem || !cJSON_IsObject(communicationItem)) throw std::runtime_error("收到错误信息。");

            	    cJSON *InstructionItem = cJSON_GetObjectItem(communicationItem, "指示");
            	    if(!InstructionItem || !cJSON_IsString(InstructionItem)) throw std::runtime_error("收到错误信息。");

            	    InstructionGot = std::string(InstructionItem->valuestring);

                }

                //--------------------------------------------------------------------------------
                /*尝试将数据保存到全局配置*/
                //对象->取脚本()->添加("简报—处理器", tmp1.open("简报—处理器"));

                std::string InstructionSend = "关闭连接";
                if (InstructionGot == "关闭连接"){
                    this->close();/*出口*/
                }
                else if	(InstructionGot == "保持联络") InstructionSend = "保持联络";
	       else {
	           this->close();
		   CommonThread::Print(InstructionGot);
	      }
                
//this->Print("prepare");
                std::string msg;
                //--------------------------------------------------------------------------------
                /*准备要发送的脚本，服务器不主动关闭连接*/
                {
                    //DScript DataReport_out;
    	            //DataReport_out.Add("简报—名字", CommonThread::GetScript()->Open("简报—名字"));
        	        //DataReport_out.Add("通信—指示", InstructionSend);

            	    cJSON *reportItem = cJSON_CreateObject();
                    std::string name = CommonThread::GetScript()->Open("简报—名字");
                	cJSON_AddItemToObject(reportItem, "名字", cJSON_CreateString(name.c_str()));

	                cJSON *communicationItem = cJSON_CreateObject();
                	cJSON_AddItemToObject(communicationItem, "指示", cJSON_CreateString(InstructionSend.c_str()));

    	            JsonString Json_DataReport_Out;
        	        cJSON_AddItemToObject(Json_DataReport_Out.get(), "简报", reportItem);
        	        cJSON_AddItemToObject(Json_DataReport_Out.get(), "通信", communicationItem);

                    msg = Json_DataReport_Out.print();
                    //this->Print(msg);
                }
                //--------------------------------------------------------------------------------
//this->Print("send");
                /*发送数据*/
                //sock->send_msg(sock->getSocket(), DataReport_out.toStr());
                Communication::send_msg(this->sock, msg);    
                    
            } catch (std::exception& e) {
                CommonThread::Print(std::string("连接断开：") + e.what());
                this->close();
            }/*通信异常*/
            
		}/*循环体*/
            

        void ClearEvent() override {
            /*程序清理阶段的操作函数。*/
            
            try {
                if (this->sock != -1) {
                    /*如果Linux服务器类型已经被设置过了
                    就将其析构，如果没有什么也不做*/
                    ::close(this->sock);
                    this->sock = -1;
                    /*执行成功*/
                    //CommonThread::Print("清除。");
                } /*删除伺服器*/

            } catch(std::exception e) {
                CommonThread::Print(std::string("清理时出错：") + e.what());
                this->stop();
            }/*清理阶段异常*/
            
        }/*清除事件*/

    
	};/*连接器*/
	
}/*qing*/
