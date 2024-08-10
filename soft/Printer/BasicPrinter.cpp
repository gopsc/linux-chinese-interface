#include "BasicPrinter.h"

namespace qing {
	
    void BasicPrinter::Print(std::string s1, std::string s2) {
        /*打印：将目标内容追加到输出池的末尾*/
        lk.lock();
        /*------------临界区------------*/
        double t = Os::get_runtime();
        this->outPool.push_back(PrinterData(t, s1, s2));
	if(this->outPool.size() > PRINTER_MAXLINE)
            this->outPool.erase(outPool.begin());
        /*-----------------------------*/
        lk.unlock();
    } //打印
	   
    std::string BasicPrinter::Get(int w, int h) {
        /*获取输出池尾部的格式内容，用于在屏幕上显示*/
        lk.lock();
        /*------------临界区------------*/
        std::string out = "";
        int l = outPool.size();
            
	/*从输出池倒序输出输出*/
        for (int i=l-1; i>=0; --i) {
            /*从输出池获取输出*/
            double t = outPool[i].t;
            std::string n = outPool[i].n;
            std::string s = outPool[i].s;
            /*组装输出池的输出*/
            std::string connectted = (n == "") ? (s + "\n") : (string::to_string(t,2) + " " + n + " " + s + "\n");
	    /*向单行插入换行符，并且堆叠*/
            out = PaperCutter::InsertLineBreak(connectted, w) + out;
            /*已经达到预定长度，跳出*/
            if (string::count_lines(out) >= h) break;
        }
        /*这一步是为了从结果中切割尾部的可视部分*/
        std::string res = PaperCutter::CutLines(out, h);
        /*-----------------------------*/
        lk.unlock();
        return res;
    } //取
        
        
}//夕
