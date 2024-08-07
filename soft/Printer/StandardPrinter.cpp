
#include "StandardPrinter.h"	
extern qing::CmdPainter *painter;
    
namespace qing {
    
    StandardPrinter::StandardPrinter(int w, int h) {
        /*构造函数，初始化打印机的高度和宽度*/
        this->w = w;
        this->h = h;
    }
    
    void StandardPrinter::Set(int w, int h) {
        /*重新设置打印机的高度和宽度*/
        this->w = w;
        this->h = h;
    }
    
    std::string StandardPrinter::Get() {
        /*获取一页预期宽高的字符串*/
        return BasicPrinter::Get(this->w, this->h);
    }

    
    void StandardPrinter::Print(std::string n, std::string s) {
        /*因为有多个线程访问打字机，需要建立一个锁去控制*/
        this->lk.lock();
        /*以设置的高度和宽度进行打印*/
        BasicPrinter::Print(n, s);

#ifdef DEBUG
        if (n != "")	std::cout << n + "  " + s << std::endl;
        else			std::cout << s << std::endl;
#else
        painter->lk.lock();
        
        std::string msg = this->Get();
        Area     *a1    = painter->a3_1;
        char      *ansi_str = string::utf8_to_ansi((char*)msg.c_str());
            
        a1->paint(BLACK32);
        a1->print(ansi_str, painter->char_w, painter->char_h, WHITE32);
        free(ansi_str);

        painter->Paint();
        painter->lk.unlock();
#endif
        this->lk.unlock();
    } //标准打印机


}//夕
