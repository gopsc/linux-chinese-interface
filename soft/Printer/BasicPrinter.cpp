#include "BasicPrinter.h"

namespace qing {
	
    void BasicPrinter::Print(std::string s1, std::string s2) {
        /*打印：将目标内容追加到输出池的末尾*/
        lk.lock();
        /*------------临界区------------*/
        double t = Os::get_runtime();
        this->outPool.push_back(PrinterData(t, s1, s2));
        /*-----------------------------*/
        lk.unlock();
    } //打印
	   
    std::string BasicPrinter::Get(int w, int h) {
        /*获取输出池尾部的格式内容，用于在屏幕上显示*/
        lk.lock();
        /*------------临界区------------*/
        std::string out = "";
        int l = outPool.size();
            
        for (int i=l-1; i>=0; --i) {
            std::string t = string::to_string(outPool[i].t, 2);
            std::string n = outPool[i].n;
            std::string s = outPool[i].s;
            
            std::string connectted = n == "" ? s + "\n" : t + " " + n + " " + s + "\n";
            out = PaperCutter::InsertLineBreak(connectted, w) + out;
            /*已经达到预定长度，跳出*/
            if (string::count_lines(out) >= h) break;
        }
            
        std::string res = PaperCutter::CutLines(out, h);
        /*-----------------------------*/
        lk.unlock();
        return res;
    } //取
        
        
}//夕
