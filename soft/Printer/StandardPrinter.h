#ifndef __H_StandardPrinter
#define __H_StandardPrinter

#include "../../type.h"
#include "../Painter/Area/Area.h"
#include "../data/string.h"
#include "../Painter/CmdPainter.h"
#include "BasicPrinter.h"

#ifdef __cplusplus
#include <mutex>


namespace qing{
    class StandardPrinter: public BasicPrinter {
        /*标准打印机类型，继承自基本打印机类型
        含有要打印的高度和宽度定义*/

        public:
            StandardPrinter(int w, int h);
            void Set(int w, int h);
            std::string Get();
            /*以设置的高度和宽度进行打印*/
            void Print(std::string n, std::string s);
    
        private:
            /*我们所要打印的字符串的宽和高*/
            int w, h;
            /*用一个锁去控制对缓冲区的写操作*/
            std::mutex lk;

    }; //标准打字机

} //qing

#endif //__cplusplus
#endif //!__H_StandardPrinter
