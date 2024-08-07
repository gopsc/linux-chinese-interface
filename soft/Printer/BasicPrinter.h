#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <mutex>
#include "../../type.h"
#include "../data/string.h"
#include "../os.h"
#include "PrinterData.h"
#include "PaperCutter.h"

namespace qing {
    
    class BasicPrinter: public Unit {
        /*基本打印机类型，用于向某种输出设备提供经过处理的字符串*/
        private:
            std::mutex lk;
            /*输出池，向量*/
            std::vector<PrinterData> outPool;
        public:
            /*打印：将目标内容追加到输出池的末尾*/
            void Print(std::string s1, std::string s2);
            /*获取输出池尾部的格式内容，用于在屏幕上显示*/
            std::string Get(int w, int h);

        
    }; //基本打字机

}//qing
