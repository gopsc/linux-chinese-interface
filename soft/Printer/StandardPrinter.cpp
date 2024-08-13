
#include "StandardPrinter.h"	

/*main.cpp*/
extern bool isDebug;

/*在main.cpp中定义的命令行绘图器，用于绘制命令行界面*/
extern qing::CmdPainter *painter;
    
namespace qing {
    
    StandardPrinter::StandardPrinter(int w, int h) {
        this->w = w; this->h = h;
    }/*构造函数，初始化打印机的高度和宽度*/
    
    void StandardPrinter::Set(int w, int h) {
        this->w = w; this->h = h;
    }/*重新设置打印机的高度和宽度*/
    
    std::string StandardPrinter::Get() {
        return BasicPrinter::Get(this->w, this->h);
    }/*获取一页预期宽高的字符串*/

    
    void StandardPrinter::Print(std::string n, std::string s) {
        /*因为有多个线程访问打字机，需要建立一个锁去控制*/
        this->lk.lock();
        /*以设置的高度和宽度进行打印*/
        BasicPrinter::Print(n, s);

        if (isDebug)
            if (n != "") std::cout << n + "  " + s << std::endl;
            else std::cout << s << std::endl;
        else{
	    /*为绘图器加锁*/
            painter->lk.lock();
            /*从绘图器获取命令行区域*/
            Area *a1 = painter->a3_1;
	    /*从超类中获取输出池尾部的信息，并且转化为ANSI编码*/
            char *ansi_str = string::utf8_to_ansi((char*)this->Get().c_str());
            /*在命令行区域上执行绘制*/
            a1->paint(BLACK32);
            a1->print(ansi_str, painter->char_w, painter->char_h, WHITE32);
            /*清理*/
            free(ansi_str);
            /*组装界面，并且刷写到屏幕*/
            painter->Paint();
            painter->lk.unlock();
	}
        this->lk.unlock();
    } //标准打印机


}//夕
