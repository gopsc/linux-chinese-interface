
#include "CommonThread.h"

/* main.c */
extern bool isDebug;
extern qing::StandardPrinter *printer;


namespace qing {
	 
    CommonThread::CommonThread(std::string name) {
        /*构造函数，完成数据域的设置。*/
        this->label = name;
        //创建线程的函数
        this->thread = new std::thread(thread_main, this);
    }
    
    CommonThread::~CommonThread(){
        if (this->thread){
            //清除线程

	    if (isDebug) std::cout << std::string("Close:\t") + this->label + "\n";

              //使线程自主退出的函数必须放到外面或者子类叶子上，
	      //否则会导致关闭时子类已经被释放，进而导致调用不了

//            if (this->chk() != SSHUT){
//                this->close();//使线程自主关闭
//            }
//            this->WaitClose();//等待线程退出
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

}
