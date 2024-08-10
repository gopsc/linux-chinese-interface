#pragma once

#include <cstring>
#include <exception>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../../network/Communication.h"
#include "../script/DScript.h"
#include "../Printer/StandardPrinter.h"
#include "CommonThread.h"
#include "Connector.h"

/*
@qing：此模块为程序的网络伺服器模块。
*/

//@楠舍科技-顾雨杭,2024-2-1。

namespace qing {

    class Reciver: public CommonThread {
        /*创建一个信使线程，相当于一个基本的服务器程序*/
    public:
        /*构造函数*/
        Reciver(StandardPrinter *printer, DScript *script, std::string name) : CommonThread(printer, script, name) {}
        /*暂时删除复制构造函数*/
        Reciver(Reciver&) = delete;
        
        void StopEvent() override {
            /*程序睡眠阶段的操作函数，什么也不做*/
            this->suspend();
        }/*静止事件*/
        
            
        void WakeEvent() override {
            /*程序设置阶段的操作函数。*/

            try {
                /*使用配置文件中设置的端口*/
                std::string port = CommonThread::GetScript()->Open("配置—接收装置—端口");
                /*输出*/
                //CommonThread::Print(std::string("端口开始监听：") + port);
                
                /*在堆上新建一个linux服务器类*/
                //server = new LinuxServer(std::stoi(port), 5);
                {
                    /*Create socket*/
                    this->sock = socket(AF_INET, SOCK_STREAM, 0);
                    if (this->sock <= 0)
                        throw std::runtime_error(std::string("Create socket: ") + strerror(errno));
                }
                {
                    //Specify the protocol family as IPv4
                    serveraddr.sin_family = AF_INET;
                    
                    //Set the IP address to any address
                    serveraddr.sin_addr.s_addr = INADDR_ANY;
                    
                    //Convert the port number to network byte order
                    serveraddr.sin_port = htons(std::stoi(port));
                }
                
                {
                    /*set port nowait*/
                    int v = 1;
                    setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
                }
                
                {
                    /* Set blocking time */
                    //@陈昊元
                    struct timeval t;
                    t.tv_sec = 2;
                    t.tv_usec = 0;
                    
                    if (setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&t, sizeof(t)) < 0)
                        throw std::runtime_error(std::string("Set timeout: ") + strerror(errno));
                        
                    if (setsockopt(this->sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&t, sizeof(t)) < 0)
                        throw std::runtime_error(std::string("Set timeout: ") + strerror(errno));
                }
		
		{
		    int sockfd;
		    int flag = 1;

		    //设置套接字选项以关闭Nagle算法
		    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
		}
                
                {
                    if (bind(this->sock, (struct sockaddr*)&(this->serveraddr), sizeof(sockaddr_in)) == -1)
                        throw std::runtime_error(std::string("Server Bind: ") + strerror(errno));
                        
                    if (listen(this->sock, 5) == -1)
                        throw std::runtime_error(std::string("Server start listen") + strerror(errno));
                }
                
                //通用线程::打印("初始化成功。");
                this->run();

            } catch (std::exception& e) {
                CommonThread::Print(std::string("程序在设置阶段出现异常：") + e.what());
                this->stop(); return;
            }/*设置阶段异常*/

        }/*配置事件*/
            
            
        void LoopEvent() override {
            /*程序运行阶段的操作函数。*/
           
            try {
            
                /* 等待客户端连接 */
                //ClientSocket *Connection = new ClientSocket(this->sock);
                sockaddr_in clientaddr; // A struct for temporarily recording client addresses
                socklen_t l = sizeof(clientaddr);
                int clientsock = accept(this->sock, (struct sockaddr*)&clientaddr, &l);
                if (clientsock == -1) throw std::runtime_error(std::string("Create client socket: ") + strerror(errno));
//this->PrintOnly(std::to_string(clientsock));
                
                /*建立交流线程*/
                std::string name = std::string("交谈") + std::to_string(++num);
                Connector *connector = new Connector(CommonThread::GetPrinter(), CommonThread::GetScript(), name, clientsock, clientaddr);
                connector->wake();
                connector->WaitStart(1);

                /*并推入连接池*/
                pool.push_back(connector); 
                
            }
            
            catch (std::exception &e) {
                usleep(100000); /*睡眠10ms，等待下一次连接*/
                //通用线程::打印("重新运行。");
            }/*接收连接失败，这里的异常是接收失败导致的，不用管*/
            
            if(this->chk() == SRUNNING){
            /*如果不放在条件分支中，将会导致段错误，不知道为什么*/

                for (int i = pool.size() - 1; i > -1; --i) {
                    /*检查连接池的长度*/
                    if (pool[i]->chk() == SSHUT) {
                        delete pool[i];
                        pool.erase(pool.begin() + i);
                        break;/*一次只删除一个元素*/
                    }/*该连接的交流已经停止，移除元素*/
                }/*维护连接池*/

	    }

        }/*循环体*/
            

        void ClearEvent() override {
            /*程序清理阶段的操作函数。*/
CommonThread::Print("清除程序启动。");
            
            try {
                
                for(int i=pool.size()-1; i>=0; i--) {
                    delete pool[i];
                }/*遍历线程池，关闭所有连接*/
                pool.clear();
                
                if (this->sock > 0) {
                    /*如果Linux服务器类型已经被设置过了
                    /就将其析构，如果没有什么也不做*/
                    ::close(this->sock);
                    this->sock = -1;
                } /*删除伺服器*/
                
                /*执行成功*/
CommonThread::Print("清除。");
            }
            
            catch (std::exception e) {
                CommonThread::Print(std::string("清理时出错：") + e.what());
                this->stop();
            }/*清理阶段异常*/
            
        }/*清除事件*/

    private:
        //这里用来存放类里面用到的变量和函数的定义
        //套接字的标识符。
        //LinuxServer* server;
        
        int sock = -1;
        sockaddr_in serveraddr;
        
        //存放客户端套接字的向量。
        std::vector<Connector*> pool;
        
        //存放一共有多少次交谈
        size_t num = 0;

        
    };/*接收装置*/
    
}/*qing*/
