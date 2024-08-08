#include "PaperCutter.h"

namespace qing {
	
	
    std::string PaperCutter::InsertLineBreak(std::string s, int w) {
        /*在目标字符串中插入换行符，使得目标字符串符合一定的最大宽度。  
        仅适用于汉字的整理，且在这里汉字和英文字母是一样大小*/
        int i=0, j=0, l1 = s.length();
            
        for (i = 0; i < l1;) {

            if (j==w) {
                s.insert(i,"\n");
                j=0; i++;
            }/*如果计数器达到了指定的宽度，就插入一个换行符*/

            /*将一个两字节字符装填到盒子*/
            char c1 = s[i];
            /*求盒子里字符的长度*/
            size_t l2 = string::count_utf8_char(c1);
            /*遍历计数器向后移动一个字符长度*/
            i+=l2;
            /*如果该字符是一个换行符，就重新计数，否则就将计数器加一*/
            if(c1 == '\n') j=0;
            else j++;
        }//遍历整个字符串

        return s;
    } //插入换行符
        
    std::string PaperCutter::CutLines(std::string s, int h) {
        /*从目标字符串尾部截取字符串，使得目标字符串符合一定的最大行数。*/
        size_t l1 = s.length(), k1 = l1, k2 = 0;
        /*遍历字符串中的每个换行符，同时进行计数
        当到达指定行数或者没有足够的行时，遍历结束*/
        for(; k1 != -1 && k2 < h; ++k2) k1 = s.rfind('\n', k1 - 1);
        /*如果找到了包含指定行数的换行符位置，就进行字符串截取*/
        if (k1 != -1) s = s.substr(k1 + 1, l1 - k1 - 1);
        return s;
    } //裁剪行

}//qing
